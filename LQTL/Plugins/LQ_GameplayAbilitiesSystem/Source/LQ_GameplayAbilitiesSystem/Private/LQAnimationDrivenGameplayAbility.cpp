// Copyright Tale Weavers


#include "LQAnimationDrivenGameplayAbility.h"

#include "LQAbilitySystemComponent.h"
#include "LQAbilitySystemInterface.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Engine/AssetManager.h"
#include "StructUtils/StructView.h"

ULQAnimationDrivenGameplayAbility::ULQAnimationDrivenGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
}

void ULQAnimationDrivenGameplayAbility::PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                    FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
	if (auto Avatar = GetAvatarActorFromActorInfo())
	{
		AvatarSkel = Cast<USkeletalMeshComponent>(Avatar->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	}
	if (ActivationPayloadData.IsValid())
	{
		if (ActivationPayloadData.GetScriptStruct() == FLQCombatAnimationAbilityPayload::StaticStruct())
		{
			FLQCombatAnimationAbilityPayload AnimationPayload = ActivationPayloadData.Get<FLQCombatAnimationAbilityPayload>();
			if (AnimationPayload.Montage.IsNull() == false && AnimationPayload.Montage.IsValid() == false)
			{
				UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(AnimationPayload.Montage.ToSoftObjectPath(),
				                                                             FStreamableDelegate::CreateUObject(this, &ULQAnimationDrivenGameplayAbility::OnMontageAssetLoaded));
			}
		}
	}
}

UAnimMontage* ULQAnimationDrivenGameplayAbility::GetContextDrivenMontage_Implementation()
{
	return ActivationPayloadData.IsValid() ? ActivationPayloadData.Get<FLQCombatAnimationAbilityPayload>().Montage.Get() : nullptr;
}

void ULQAnimationDrivenGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                        const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	DrivenMontage = GetContextDrivenMontage();
	if (!DrivenMontage)
	{
		ActivateAbilityDeferred.BindUObject(this, &ULQAnimationDrivenGameplayAbility::ActivateAbility, Handle, ActorInfo, ActivationInfo, TriggerEventData);
		return;
	}

	if (DrivenMontage && AvatarSkel && bAutoPlayMontageOnActivation)
	{
		if (auto ASC = Cast<ULQAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo()))
		{
			ASC->AddMontageToAbilitySpecHandle(Handle, TObjectKey<UAnimMontage>(DrivenMontage));
		}
		UAbilityTask_PlayMontageAndWait* PlayMontageAndWait = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, DrivenMontage);
		PlayMontageAndWait->OnCompleted.AddUniqueDynamic(this, &ULQAnimationDrivenGameplayAbility::OnMontageCompleted);
		PlayMontageAndWait->OnBlendOut.AddUniqueDynamic(this, &ULQAnimationDrivenGameplayAbility::OnMontageBlendOut);
		PlayMontageAndWait->OnInterrupted.AddUniqueDynamic(this, &ULQAnimationDrivenGameplayAbility::OnMontageInterrupted);
		PlayMontageAndWait->OnCancelled.AddUniqueDynamic(this, &ULQAnimationDrivenGameplayAbility::OnMontageCancelled);
		PlayMontageAndWait->ReadyForActivation();
	}
}

void ULQAnimationDrivenGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (auto ASC = Cast<ULQAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo()))
	{
		ASC->RemoveMontageToAbilitySpecHandle(Handle);
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void ULQAnimationDrivenGameplayAbility::OnMontageAssetLoaded()
{
	if (ActivateAbilityDeferred.IsBound())
	{
		ActivateAbilityDeferred.Execute();
		ActivateAbilityDeferred.Unbind();
	}
}

void ULQAnimationDrivenGameplayAbility::OnMontageCancelled_Implementation()
{
	//native implementation
	if (EndAbilityOnMontageEventPolicy & EEndAbilityOnMontageEventPolicy::EAMP_OnCancelled)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void ULQAnimationDrivenGameplayAbility::OnMontageInterrupted_Implementation()
{
	//native implementation
	if (EndAbilityOnMontageEventPolicy & EEndAbilityOnMontageEventPolicy::EAMP_OnInterrupted)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void ULQAnimationDrivenGameplayAbility::OnMontageBlendOut_Implementation()
{
	//native implementation
	if (EndAbilityOnMontageEventPolicy & EEndAbilityOnMontageEventPolicy::EAMP_OnBlendOut)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void ULQAnimationDrivenGameplayAbility::OnMontageCompleted_Implementation()
{
	//native implementation
	if (EndAbilityOnMontageEventPolicy & EEndAbilityOnMontageEventPolicy::EAMP_OnCompleted)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}
