// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "LQAbilitySystemTypes.h"
#include "StructUtils/InstancedStruct.h"
#include "LQAbilitySystemComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAbilityEndedSignature, FGameplayAbilitySpecHandle, Handle, UGameplayAbility*, Ability, bool, bWasCancelled);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LQ_GAMEPLAYABILITIESSYSTEM_API ULQAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	friend class ULQGameplayAbility;
	friend class ULQAnimationDrivenGameplayAbility;
	friend class ULQAbilitySystemLibrary;

public:
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;
	virtual void NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled) override;
	//called on Damage gameplay effect is applied on self (Received damage)
	void NotifyPostApplyDamageOnSelf(float DamageApplied, const FGameplayEffectSpec& EffectSpec);

	//called on Damage gameplay effect is succeeded to applied, but before the real application 
	void NotifyBeforeApplyDamageOnSelf(const FGameplayEffectSpec& EffectSpec);

	//called on the attacker to apply damage to the target
	void ApplyDamageGameplayEffectToTarget(const FGameplayAbilitySpecHandle& AbilitySpecHandle, ULQAbilitySystemComponent* TargetAsc, const FHitResult& HitResult);
	void ExtractExternalDataOnApplyDamage(TArray<FPreApplyDamageData>& ExternalData);
	FGameplayAbilitySpecHandle GetAbilitySpecHandleFromMontage(class UAnimMontage* InMontage);
	void PushAbilityPayload(const FGameplayAbilitySpecHandle& Handle, const TInstancedStruct<FLQAbilityPayloadDataBase>& PayloadData);
	bool PopAbilityPayload(const FGameplayAbilitySpecHandle& Handle, TInstancedStruct<FLQAbilityPayloadDataBase>& OutPayloadData);
	void PushExternalAttackDataFromAbilitySpecHandle(const FGameplayAbilitySpecHandle& AbilitySpecHandle, const TInstancedStruct<FExternalAttackData>& Data);
	void GetExternalAttackDataByAbilitySpecHandle(const FGameplayAbilitySpecHandle& AbilitySpecHandle, TFunction<bool(const TInstancedStruct<FExternalAttackData>&)> Filter,
	                                              TArray<TInstancedStruct<FExternalAttackData>>& OutData);
	void PopLastExternalAttackData(TFunction<bool(const TInstancedStruct<FExternalAttackData>&)> Filter, TArray<TInstancedStruct<FExternalAttackData>>& OutData);
	void RemoveExternalAttackDataOfSpecHandle(const FGameplayAbilitySpecHandle& AbilitySpecHandle);

	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "On Ability Ended"))
	FOnAbilityEndedSignature OnAbilityEndedExposed;

protected:
	ULQAbilitySystemComponent();
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category="LQ|AbilitySystemComponent")
	void LQInitAbilityInfo(AActor* InOwnerActor, AActor* InAvatarActor);

	void AddMontageToAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle, const TObjectKey<UAnimMontage>& MontageObjectKey);
	void RemoveMontageToAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);

private:
	TMap<FGameplayAbilitySpecHandle, TInstancedStruct<FLQAbilityPayloadDataBase>> AbilityPayloads;
	TMultiMap<FGameplayAbilitySpecHandle, TObjectKey<UAnimMontage>> AbilityHandleMontageMap;

	//array to store data from external sources: combo, effects, etc
	//add on ability is activated and PopPayload, get on applying damage
	TMultiMap<FGameplayAbilitySpecHandle, TInstancedStruct<FExternalAttackData>> ExternalAttackData;
};
