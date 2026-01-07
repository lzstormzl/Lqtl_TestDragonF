// Copyright Tale Weavers

#include "FloatingTextManager.h"
#include "FloatingTextSettings.h"
#include "FloatingTextWidget.h"
#include "Blueprint/UserWidget.h"
#include "TimerManager.h"

UFloatingTextManager::UFloatingTextManager()
{
	PrimaryComponentTick.bCanEverTick = true;
	Space = EWidgetSpace::Screen;
	bDrawAtDesiredSize = true;
}

UFloatingTextManager* UFloatingTextManager::GetFromActor(AActor* Actor)
{
	if (!Actor)
	{
		return nullptr;
	}

	return Actor->FindComponentByClass<UFloatingTextManager>();
}

void UFloatingTextManager::BeginPlay()
{
	Super::BeginPlay();
	InitializePools();
}

void UFloatingTextManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Clear all timers
	for (auto& PoolPair : WidgetPools)
	{
		for (FFloatingTextWidgetInstance& Instance : PoolPair.Value)
		{
			if (Instance.DeactivationTimer.IsValid())
			{
				GetWorld()->GetTimerManager().ClearTimer(Instance.DeactivationTimer);
			}
		}
	}

	Super::EndPlay(EndPlayReason);
}

void UFloatingTextManager::InitializePools()
{
	const UFloatingTextSettings* Settings = UFloatingTextSettings::Get();
	if (!Settings)
	{
		return;
	}

	// Pre-warm pools for all registered widget types
	for (const auto& Pair : Settings->WidgetTemplateMap)
	{
		TSubclassOf<UFloatingTextWidget> FloatingTextWidgetClass = Pair.Value;
		if (!FloatingTextWidgetClass)
		{
			continue;
		}

		TArray<FFloatingTextWidgetInstance>& Pool = WidgetPools.FindOrAdd(FloatingTextWidgetClass);
		Pool.Reserve(Settings->DefaultPoolSize);

		// Create initial pool
		for (int32 i = 0; i < Settings->DefaultPoolSize; ++i)
		{
			FFloatingTextWidgetInstance Instance = CreateWidgetInstance(FloatingTextWidgetClass);
			Pool.Add(Instance);
		}

		ActiveWidgetCounts.Add(FloatingTextWidgetClass, 0);
	}

	// Also pre-warm default widget class if set
	if (!Settings->DefaultWidgetClass.IsNull())
	{
		TSubclassOf<UFloatingTextWidget> DefaultClass = Settings->DefaultWidgetClass.LoadSynchronous();
		if (DefaultClass && !WidgetPools.Contains(DefaultClass))
		{
			TArray<FFloatingTextWidgetInstance>& Pool = WidgetPools.FindOrAdd(DefaultClass);
			Pool.Reserve(Settings->DefaultPoolSize);

			for (int32 i = 0; i < Settings->DefaultPoolSize; ++i)
			{
				FFloatingTextWidgetInstance Instance = CreateWidgetInstance(DefaultClass);
				Pool.Add(Instance);
			}

			ActiveWidgetCounts.Add(DefaultClass, 0);
		}
	}
}

FFloatingTextWidgetInstance UFloatingTextManager::CreateWidgetInstance(TSubclassOf<UFloatingTextWidget> FloatingWidgetClass)
{
	FFloatingTextWidgetInstance Instance;

	if (!FloatingWidgetClass)
	{
		return Instance;
	}

	// Create widget
	Instance.Widget = CreateWidget<UFloatingTextWidget>(GetWorld(), FloatingWidgetClass);

	return Instance;
}

FFloatingTextWidgetInstance* UFloatingTextManager::AcquireWidget(TSubclassOf<UFloatingTextWidget> FloatingWidgetClass)
{
	TArray<FFloatingTextWidgetInstance>& Pool = WidgetPools.FindOrAdd(FloatingWidgetClass);
	const UFloatingTextSettings* Settings = UFloatingTextSettings::Get();

	if (!Settings)
	{
		return nullptr;
	}

	// Find available widget in pool
	for (FFloatingTextWidgetInstance& Instance : Pool)
	{
		if (!Instance.bIsActive)
		{
			Instance.bIsActive = true;
			ActiveWidgetCounts[FloatingWidgetClass]++;
			return &Instance;
		}
	}

	// Pool exhausted - check if we can create more
	if (Pool.Num() < Settings->MaxPoolSize)
	{
		FFloatingTextWidgetInstance NewInstance = CreateWidgetInstance(FloatingWidgetClass);
		NewInstance.bIsActive = true;
		Pool.Add(NewInstance);
		ActiveWidgetCounts[FloatingWidgetClass]++;
		return &Pool.Last();
	}

	// Hard limit reached - reuse oldest active widget
	UE_LOG(LogFloatingTextSystem, Warning, TEXT("Widget pool exhausted for class %s, reusing oldest"), *FloatingWidgetClass->GetName());

	// Find oldest active widget (first active in array)
	for (FFloatingTextWidgetInstance& Instance : Pool)
	{
		if (Instance.bIsActive)
		{
			// Clear existing timer
			if (Instance.DeactivationTimer.IsValid())
			{
				GetWorld()->GetTimerManager().ClearTimer(Instance.DeactivationTimer);
			}
			return &Instance;
		}
	}

	return nullptr;
}

void UFloatingTextManager::ReleaseWidget(FFloatingTextWidgetInstance* WidgetInstance, TSubclassOf<UFloatingTextWidget> FloatingTextWidgetClass)
{
	if (!WidgetInstance)
	{
		return;
	}

	WidgetInstance->bIsActive = false;
	WidgetInstance->DeactivationTimer.Invalidate();

	if (int32* Count = ActiveWidgetCounts.Find(FloatingTextWidgetClass))
	{
		(*Count)--;
	}
}

TSubclassOf<UFloatingTextWidget> UFloatingTextManager::ResolveWidgetClass(const FFloatingTextDataBase& TextData)
{
	const UFloatingTextSettings* Settings = UFloatingTextSettings::Get();
	if (!Settings)
	{
		return nullptr;
	}

	// Determine floating text type from data
	EFloatingTextType TextType = TextData.TextType;

	// Look up widget class by text type
	if (TSubclassOf<UFloatingTextWidget> const* FoundClass = Settings->WidgetTemplateMap.Find(TextType))
	{
		return *FoundClass;
	}

	// Fall back to default
	if (!Settings->DefaultWidgetClass.IsNull())
	{
		return Settings->DefaultWidgetClass.LoadSynchronous();
	}

	UE_LOG(LogFloatingTextSystem, Warning, TEXT("No widget class found for text type, using default"));
	return nullptr;
}

void UFloatingTextManager::SpawnFloatingText(const TInstancedStruct<FFloatingTextDataBase>& TextData)
{
	if (!TextData.IsValid())
	{
		return;
	}

	// Resolve widget class
	TSubclassOf<UFloatingTextWidget> FloatingWidgetClass = ResolveWidgetClass(TextData.Get<FFloatingTextDataBase>());
	if (!FloatingWidgetClass)
	{
		return;
	}

	// Acquire widget from pool
	FFloatingTextWidgetInstance* Instance = AcquireWidget(FloatingWidgetClass);
	if (!Instance || Instance->Widget == nullptr)
	{
		return;
	}

	// Initialize widget with data
	Instance->Widget->InitializeWithData(TextData);
	Instance->Widget->OnActivated();

	// Set widget
	SetWidget(Instance->Widget);

	// Set deactivation timer
	const UFloatingTextSettings* Settings = UFloatingTextSettings::Get();
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUObject(this, &UFloatingTextManager::OnWidgetDeactivationTimer, Instance, FloatingWidgetClass);
	GetWorld()->GetTimerManager().SetTimer(Instance->DeactivationTimer, TimerDelegate, Settings ? Settings->DisplayDuration : 0.2f, false);
}

void UFloatingTextManager::OnWidgetDeactivationTimer(FFloatingTextWidgetInstance* WidgetInstance, TSubclassOf<UFloatingTextWidget> FloatingTextWidgetClass)
{
	if (!WidgetInstance)
	{
		return;
	}

	// Deactivate widget
	WidgetInstance->Widget->OnDeactivated();
	WidgetInstance->Widget->ResetWidget();

	// Release to pool
	ReleaseWidget(WidgetInstance, FloatingTextWidgetClass);
}
