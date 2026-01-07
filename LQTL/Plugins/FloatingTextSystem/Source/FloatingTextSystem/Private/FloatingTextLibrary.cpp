// Copyright Tale Weavers

#include "FloatingTextLibrary.h"
#include "FloatingTextManager.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GameModeBase.h"
#include "EngineUtils.h"
#include "Internationalization/Text.h"

void UFloatingTextLibrary::SpawnFloatingText(UObject* WorldContextObject, const TInstancedStruct<FFloatingTextDataBase>& TextData)
{
	UFloatingTextManager* Manager = FindFloatingTextManager(WorldContextObject);
	if (Manager)
	{
		Manager->SpawnFloatingText(TextData);
	}
}

UFloatingTextManager* UFloatingTextLibrary::FindFloatingTextManager(UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return nullptr;
	}

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return nullptr;
	}

	// Try PlayerController first
	if (AActor* Actor = Cast<AActor>(WorldContextObject))
	{
		if (UFloatingTextManager* Manager = Actor->FindComponentByClass<UFloatingTextManager>())
		{
			return Manager;
		}
	}

	UE_LOG(LogFloatingTextSystem, Warning, TEXT("FloatingTextManager not found on PlayerController or GameMode, searching all actors"));

	for (TActorIterator<AActor> It(World); It; ++It)
	{
		if (UFloatingTextManager* Manager = It->FindComponentByClass<UFloatingTextManager>())
		{
			return Manager;
		}
	}

	UE_LOG(LogFloatingTextSystem, Error, TEXT("FloatingTextManager not found in world"));
	return nullptr;
}
