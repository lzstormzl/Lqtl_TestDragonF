// Copyright Tale Weavers


#include "LQHitboxManager.h"

#include "LQHitboxAnimNotifyState.h"
#include "OnHitBehaviorBase.h"
#include "StructUtils/StructView.h"

bool bShowDebug = false;
FAutoConsoleCommandWithWorldAndArgs LQShowHitboxDebugCommand(TEXT("LQ.Debug.ShowHitbox"),TEXT("Show hitbox debug info"),
                                                             FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
                                                             {
	                                                             bShowDebug = !bShowDebug;
                                                             })
);


// Sets default values for this component's properties
ULQHitboxManager::ULQHitboxManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void ULQHitboxManager::GetCurrentDebugHitboxSettings(FHitboxDebugSettings& OutSettings)
{
	auto DebugType = bShowDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
	DebugSetting.DrawDebugType = DebugType;
	OutSettings = DebugSetting;
}


ULQHitboxManager* ULQHitboxManager::GetFromActor(AActor* Actor)
{
	if (Actor)
	{
		return Actor->FindComponentByClass<ULQHitboxManager>();
	}
	return nullptr;
}


// Called when the game starts
void ULQHitboxManager::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void ULQHitboxManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void ULQHitboxManager::ReceiveHitboxNotify(const FGameplayAbilitySpecHandle& AbilitySpecHandle, const FHitResult& HitResult, TConstStructView<FHitboxAttackData> HitboxAttackData)
{
	UE_LOGFMT(LogLQHitboxSystem, Log, "Hitbox hit actor: {0}, component: {1}", UKismetSystemLibrary::GetDisplayName(HitResult.GetActor()), GetNameSafe(HitResult.GetComponent()));

	if (!HitboxAttackData.IsValid())
		HitboxAttackData = TConstStructView<FHitboxAttackData>(FHitboxAttackData());
	if (HitResult.GetComponent())
	{
		auto FoundBehavior = OnHitBehaviorByObjectType.Find(HitResult.GetComponent()->GetCollisionProfileName());
		if (FoundBehavior && FoundBehavior->Get())
		{
			auto Behavior = NewObject<UOnHitBehaviorBase>(this, &(*FoundBehavior->Get()), TEXT("OnHitBehavior"), RF_Transient);
			if (Behavior)
			{
				Behavior->ExecuteOnHit(GetOwner(), AbilitySpecHandle, HitResult, TInstancedStruct<FHitboxAttackData>(HitboxAttackData));
			}
		}
	}
}

#if WITH_EDITOR
TArray<FName> ULQHitboxManager::GetCollisionProfiles()
{
	TArray<TSharedPtr<FName>> SharedNames;
	UCollisionProfile::GetProfileNames(SharedNames);

	TArray<FName> Names;
	Names.Reserve(SharedNames.Num());
	for (const TSharedPtr<FName>& SharedName : SharedNames)
	{
		if (const FName* Name = SharedName.Get())
		{
			Names.Add(*Name);
		}
	}
	return Names;
}
#endif
