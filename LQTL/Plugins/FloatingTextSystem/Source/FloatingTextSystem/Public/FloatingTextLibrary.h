// Copyright Tale Weavers

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FloatingTextTypes.h"
#include "FloatingTextLibrary.generated.h"

/**
 * Blueprint function library for floating text system
 */
UCLASS()
class FLOATINGTEXTSYSTEM_API UFloatingTextLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Spawn floating text
	 * Automatically finds FloatingTextManager in world
	 */
	UFUNCTION(BlueprintCallable, Category = "Floating Text", meta = (WorldContext = "WorldContextObject"))
	static void SpawnFloatingText(UObject* WorldContextObject, const TInstancedStruct<FFloatingTextDataBase>& TextData);

private:
	/**
	 * Find FloatingTextManager in world
	 * Checks: PlayerController -> GameMode -> all actors (as fallback)
	 */
	static class UFloatingTextManager* FindFloatingTextManager(UObject* WorldContextObject);
};
