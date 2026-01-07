// Copyright Tale Weavers

#pragma once

#include "CoreMinimal.h"
#include "LQAbilitySystemTypes.h"
#include "Abilities/GameplayAbility.h"
#include "StructUtils/InstancedStruct.h"
#include "LQGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class LQ_GAMEPLAYABILITIESSYSTEM_API ULQGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

protected:
	virtual void PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                         const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
	                         const FGameplayEventData* TriggerEventData = nullptr) override;
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION(BlueprintImplementableEvent)
	void ReceiveInputPressed();

	UFUNCTION(BlueprintImplementableEvent)
	void ReceiveInputReleased();

	UFUNCTION(BlueprintImplementableEvent)
	void ReceiveOnGiveAbility();

	TInstancedStruct<FLQAbilityPayloadDataBase> ActivationPayloadData;

private:
	void GetActivationPayloadFromAsc(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo);
};
