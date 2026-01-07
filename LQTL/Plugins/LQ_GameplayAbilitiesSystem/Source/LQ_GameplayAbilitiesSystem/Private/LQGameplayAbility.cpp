// Copyright Tale Weavers


#include "LQGameplayAbility.h"

#include "LQAbilitySystemComponent.h"

void ULQGameplayAbility::GetActivationPayloadFromAsc(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo)
{
	if (auto LQASC = Cast<ULQAbilitySystemComponent>(ActorInfo->AbilitySystemComponent))
	{
		LQASC->PopAbilityPayload(Handle, ActivationPayloadData);
	}
}

void ULQGameplayAbility::PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                     const FGameplayAbilityActivationInfo ActivationInfo,
                                     FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
                                     const FGameplayEventData* TriggerEventData)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
	GetActivationPayloadFromAsc(Handle, ActorInfo);
	if (ActivationPayloadData.IsValid() && ActivationPayloadData.GetScriptStruct()->IsChildOf(FLQAbilityPayloadDataBase::StaticStruct()))
	{
		// auto Payload = ActivationPayloadData.Get<FLQAbilityPayloadDataBase>();
		// SetAssetTags(FGameplayTagContainer(Payload.AbilityTag));
	}
}

void ULQGameplayAbility::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                      const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
	ReceiveInputPressed();
}

void ULQGameplayAbility::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                       const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	ReceiveInputReleased();
}

void ULQGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
	ReceiveOnGiveAbility();
}

void ULQGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (auto ASC = Cast<ULQAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo()))
	{
		ASC->RemoveExternalAttackDataOfSpecHandle(Handle);
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
