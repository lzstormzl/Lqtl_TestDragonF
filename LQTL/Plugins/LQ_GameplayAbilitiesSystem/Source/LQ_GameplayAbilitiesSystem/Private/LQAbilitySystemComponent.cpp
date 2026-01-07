// Fill out your copyright notice in the Description page of Project Settings.


#include "LQAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "LQAbilitySystemInterface.h"
#include "LQGameplayAbilitiesSettings.h"
#include "StructUtils/StructView.h"


// Sets default values for this component's properties
ULQAbilitySystemComponent::ULQAbilitySystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void ULQAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();
}

void ULQAbilitySystemComponent::PushAbilityPayload(const FGameplayAbilitySpecHandle& Handle,
                                                   const TInstancedStruct<FLQAbilityPayloadDataBase>& PayloadData)
{
	if (AbilityPayloads.Contains(Handle))
	{
		AbilityPayloads[Handle] = PayloadData;
	}
	else
	{
		AbilityPayloads.Add(Handle, PayloadData);
	}
}

void ULQAbilitySystemComponent::PushExternalAttackDataFromAbilitySpecHandle(const FGameplayAbilitySpecHandle& AbilitySpecHandle,
                                                                            const TInstancedStruct<FExternalAttackData>& Data)
{
	if (Data.IsValid())
	{
		ExternalAttackData.Add(AbilitySpecHandle, Data);
	}
}

void ULQAbilitySystemComponent::GetExternalAttackDataByAbilitySpecHandle(const FGameplayAbilitySpecHandle& AbilitySpecHandle,
                                                                         TFunction<bool(const TInstancedStruct<FExternalAttackData>&)> Filter,
                                                                         TArray<TInstancedStruct<FExternalAttackData>>& OutData)
{
	TArray<TInstancedStruct<FExternalAttackData>> Temp;
	ExternalAttackData.MultiFind(AbilitySpecHandle, Temp);
	if (Filter.IsSet())
	{
		for (const TInstancedStruct<FExternalAttackData>& Data : Temp)
		{
			if (!Filter.IsSet() || Filter(Data))
			{
				OutData.Add(Data);
			}
		}
	}
}

void ULQAbilitySystemComponent::PopLastExternalAttackData(TFunction<bool(const TInstancedStruct<FExternalAttackData>&)> Filter,
                                                          TArray<TInstancedStruct<FExternalAttackData>>& OutData)
{
	TArray<FGameplayAbilitySpecHandle> OutKeys;
	ExternalAttackData.GenerateKeyArray(OutKeys);

	FGameplayAbilitySpecHandle LastHandle = OutKeys.Last();
	ExternalAttackData.MultiFind(LastHandle, OutData);
	ExternalAttackData.Remove(LastHandle);
}

void ULQAbilitySystemComponent::RemoveExternalAttackDataOfSpecHandle(const FGameplayAbilitySpecHandle& AbilitySpecHandle)
{
	ExternalAttackData.Remove(AbilitySpecHandle);
}

bool ULQAbilitySystemComponent::PopAbilityPayload(const FGameplayAbilitySpecHandle& Handle,
                                                  TInstancedStruct<FLQAbilityPayloadDataBase>& OutPayloadData)
{
	if (TInstancedStruct<FLQAbilityPayloadDataBase>* FoundPayload = AbilityPayloads.Find(Handle))
	{
		OutPayloadData = *FoundPayload;

		if (OutPayloadData.IsValid())
		{
			PushExternalAttackDataFromAbilitySpecHandle(Handle, OutPayloadData.Get<FLQAbilityPayloadDataBase>().ExternalAttackData);
			AbilityPayloads.Remove(Handle);
			return true;
		}
	}
	return false;
}

void ULQAbilitySystemComponent::LQInitAbilityInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	InitAbilityActorInfo(InOwnerActor, InAvatarActor);
}

void ULQAbilitySystemComponent::AddMontageToAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle,
                                                              const TObjectKey<UAnimMontage>& MontageObjectKey)
{
	AbilityHandleMontageMap.Add(Handle, MontageObjectKey);
}

void ULQAbilitySystemComponent::RemoveMontageToAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	AbilityHandleMontageMap.Remove(Handle);
}

void ULQAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);
	if (InOwnerActor->Implements<ULQAbilitySystemInterface>())
	{
		ILQAbilitySystemInterface::Execute_OnAbilitySystemInitialized(InOwnerActor);
	}
	if (InAvatarActor->Implements<ULQAbilitySystemInterface>())
	{
		ILQAbilitySystemInterface::Execute_OnAbilitySystemInitialized(InAvatarActor);
	}
}

void ULQAbilitySystemComponent::ExtractExternalDataOnApplyDamage(TArray<FPreApplyDamageData>& ExternalData)
{
	TFunction<bool(const TInstancedStruct<FExternalAttackData>&)> TestFilter = [this](const TInstancedStruct<FExternalAttackData>& Data)
	{
		return Data.IsValid() && Data.GetScriptStruct()->IsChildOf(FPreApplyDamageData::StaticStruct());
	};
	FGameplayAbilitySpecHandle LastHandle = ActivatableAbilities.Items.Last().Handle;
	TArray<TInstancedStruct<FExternalAttackData>> OutExternalData;
	GetExternalAttackDataByAbilitySpecHandle(LastHandle, TestFilter, OutExternalData);

	for (const TInstancedStruct<FExternalAttackData>& Temp : OutExternalData)
	{
		ExternalData.Add(Temp.Get<FPreApplyDamageData>());
	}
}

void ULQAbilitySystemComponent::ApplyDamageGameplayEffectToTarget(const FGameplayAbilitySpecHandle& AbilitySpecHandle, ULQAbilitySystemComponent* TargetAsc,
                                                                  const FHitResult& HitResult)
{
	if (auto AbilitySetting = GetDefault<ULQGameplayAbilitiesSettings>())
	{
		if (auto GEDamageInstance = AbilitySetting->GE_DamageInstance)
		{
			TMap<FGameplayTag, float> ExternalSetByCallerMap;
			TArray<TInstancedStruct<FExternalAttackData>> OutExternalData;
			GetExternalAttackDataByAbilitySpecHandle(AbilitySpecHandle, [this](const TInstancedStruct<FExternalAttackData>& Data)
			{
				return Data.IsValid() && Data.GetScriptStruct()->IsChildOf(FPreApplyDamageData::StaticStruct());
			}, OutExternalData);

			EExternalAttackDataSource LastExternalAttackDataSource = EExternalAttackDataSource::EADS_MAX;
			for (const TInstancedStruct<FExternalAttackData>& Temp : OutExternalData)
			{
				if (Temp.IsValid())
				{
					auto Data = Temp.Get<FPreApplyDamageData>();
					Data.PrintDebug();
					Temp.Get<FPreApplyDamageData>().CollectSetByCallerData(LastExternalAttackDataSource, ExternalSetByCallerMap);
					LastExternalAttackDataSource = Data.MyDataSource;
				}
			}

			FGameplayEffectContextHandle EffectContext = MakeEffectContext();
			EffectContext.AddHitResult(HitResult);
			FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(GEDamageInstance, 1.0f, EffectContext);
			for (const TPair<FGameplayTag, float>& SetByCaller : ExternalSetByCallerMap)
			{
				UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, SetByCaller.Key, SetByCaller.Value);
			}

			if (SpecHandle.IsValid())
			{
				ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetAsc);
			}
		}
	}
}

FGameplayAbilitySpecHandle ULQAbilitySystemComponent::GetAbilitySpecHandleFromMontage(class UAnimMontage* InMontage)
{
	auto Found = AbilityHandleMontageMap.FindKey(InMontage);
	if (Found)
		return *Found;
	return FGameplayAbilitySpecHandle();
}


void ULQAbilitySystemComponent::NotifyPostApplyDamageOnSelf(float DamageApplied, const FGameplayEffectSpec& EffectSpec)
{
	if (auto Avatar = GetAvatarActor())
	{
		TArray<TInstancedStruct<FExternalAttackData>> PostApplyDamageData;
		PopLastExternalAttackData([this](const TInstancedStruct<FExternalAttackData>& Data)
		{
			return Data.IsValid() && Data.GetScriptStruct()->IsChildOf(FPostApplyDamageData::StaticStruct());
		}, PostApplyDamageData);
		if (Avatar->Implements<ULQAbilitySystemInterface>())
		{
			ILQAbilitySystemInterface::Execute_OnPostApplyDamageInstance(Avatar, DamageApplied, EffectSpec.GetEffectContext(), PostApplyDamageData);
		}
	}
}

void ULQAbilitySystemComponent::NotifyBeforeApplyDamageOnSelf(const FGameplayEffectSpec& EffectSpec)
{
	if (auto Avatar = GetAvatarActor())
	{
		if (Avatar->Implements<ULQAbilitySystemInterface>())
		{
			ILQAbilitySystemInterface::Execute_OnBeforeApplyDamageInstance(Avatar, EffectSpec.GetEffectContext());
		}
	}
}

void ULQAbilitySystemComponent::NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled)
{
	Super::NotifyAbilityEnded(Handle, Ability, bWasCancelled);
	if (OnAbilityEndedExposed.IsBound())
	{
		OnAbilityEndedExposed.Broadcast(Handle, Ability, bWasCancelled);
	}
}
