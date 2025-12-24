// Copyright Tale Weavers

#pragma once

#include "CoreMinimal.h"
#include "HLSLTypeAliases.h"
#include "LQGameplayAbility.h"
#include "LQAnimationDrivenGameplayAbility.generated.h"

/**
 * 
 */

UENUM(BlueprintType, meta=(Bitflags, UseEnumValuesAsMaskValuesInEditor=true))
enum EEndAbilityOnMontageEventPolicy : uint8
{
	EAMP_OnCompleted = 1 << 0 UMETA(DisplayName = "Completed", ToolTip = "End ability when montage completed"),
	EAMP_OnBlendOut = 1 << 1 UMETA(DisplayName = "Blend Out", ToolTip = "End ability when montage blends out"),
	EAMP_OnInterrupted = 1 << 2 UMETA(DisplayName = "Interrupted", ToolTip = "End ability when montage is interrupted"),
	EAMP_OnCancelled = 1 << 3 UMETA(DisplayName = "Cancelled", ToolTip = "End ability when montage is cancelled"),
	EAMP_None = 0 UMETA(Hidden, DisplayName = "None", ToolTip = "Do not end ability on any montage event"),
};

ENUM_CLASS_FLAGS(EEndAbilityOnMontageEventPolicy)

DECLARE_DELEGATE(FActivateAbilityDeferredDelegate);
UCLASS()
class LQ_GAMEPLAYABILITIESSYSTEM_API ULQAnimationDrivenGameplayAbility : public ULQGameplayAbility
{
	GENERATED_BODY()

protected:
	ULQAnimationDrivenGameplayAbility();
	virtual void PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	                         FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData = nullptr) override;
	UFUNCTION(BlueprintNativeEvent)
	UAnimMontage* GetContextDrivenMontage();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;
	void OnMontageAssetLoaded();
	UFUNCTION(BlueprintNativeEvent)
	void OnMontageCompleted();
	UFUNCTION(BlueprintNativeEvent)
	void OnMontageBlendOut();
	UFUNCTION(BlueprintNativeEvent)
	void OnMontageInterrupted();
	UFUNCTION(BlueprintNativeEvent)
	void OnMontageCancelled();

	UPROPERTY(EditDefaultsOnly, Category="Configurable")
	bool bAutoPlayMontageOnActivation = true;

	UPROPERTY(EditDefaultsOnly, Category="Configurable", meta=(Bitmask, BitmaskEnum="/Script/LQ_GameplayAbilitiesSystem.EEndAbilityOnMontageEventPolicy"))
	uint8 EndAbilityOnMontageEventPolicy;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> AvatarSkel;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAnimMontage> DrivenMontage;

	FActivateAbilityDeferredDelegate ActivateAbilityDeferred;
};
