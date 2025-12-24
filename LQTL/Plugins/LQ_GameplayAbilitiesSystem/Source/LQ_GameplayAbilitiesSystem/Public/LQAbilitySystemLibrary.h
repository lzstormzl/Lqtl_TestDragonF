// Copyright Tale Weavers

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "StructUtils/InstancedStruct.h"
#include "LQAbilitySystemLibrary.generated.h"

/**
 * 
 */
UCLASS()
class LQ_GAMEPLAYABILITIESSYSTEM_API ULQAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static bool TryActivateAbilityWithPayload(UAbilitySystemComponent* ASC, TSubclassOf<class UGameplayAbility> AbilityClass,
	                                          TInstancedStruct<FLQAbilityPayloadDataBase> PayloadData);
};
