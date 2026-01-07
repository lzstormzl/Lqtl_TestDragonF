// Copyright Tale Weavers

#pragma once

#include "CoreMinimal.h"
#include "LQAbilitySystemTypes.h"
#include "UObject/Interface.h"
#include "LQAbilitySystemInterface.generated.h"

struct FGameplayEffectContextHandle;
// This class does not need to be modified.
UINTERFACE()
class ULQAbilitySystemInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LQ_GAMEPLAYABILITIESSYSTEM_API ILQAbilitySystemInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent)
	void OnAbilitySystemInitialized();

	UFUNCTION(BlueprintNativeEvent)
	void OnBeforeApplyDamageInstance(const FGameplayEffectContextHandle& EffectContext);

	UFUNCTION(BlueprintNativeEvent)
	void OnPostApplyDamageInstance(float AppliedDamage, const FGameplayEffectContextHandle& EffectContext,
	                               const TArray<TInstancedStruct<FExternalAttackData>>& PostApplyDamageData);
};
