// Copyright Tale Weavers

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"
#include "GeneratedTags/SharedGameplayTags_DT_CommonTags_GameplayTagsExported.h"
#include "GeneratedTags/SharedGameplayTags_DT_HitReactionTags_GameplayTagsExported.h"
#include "StructUtils/InstancedStruct.h"
#include "UObject/Object.h"
#include "LQAbilitySystemTypes.generated.h"

namespace LQSetByCaller
{
	LQ_GAMEPLAYABILITIESSYSTEM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(ComboAttackMultiplier);
	LQ_GAMEPLAYABILITIESSYSTEM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AttackTypeMultiplier);
	LQ_GAMEPLAYABILITIESSYSTEM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AbsoluteDamage);
}

UENUM(BlueprintType)
enum class EExternalAttackDataSource : uint8
{
	EADS_ComboGraph = 0 UMETA(DisplayName = "Combo Graph"),
	EADS_MontageHitbox = 1 UMETA(DisplayName = "Montage Hitbox"),
	EADS_MAX UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct LQ_GAMEPLAYABILITIESSYSTEM_API FExternalAttackData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	EExternalAttackDataSource MyDataSource;

	virtual ~FExternalAttackData() = default;
	virtual void CollectSetByCallerData(EExternalAttackDataSource InSource, TMap<FGameplayTag, float>& OutMap) const { return; }
	virtual void PrintDebug() const { return; }
};

USTRUCT(BlueprintType)
struct LQ_GAMEPLAYABILITIESSYSTEM_API FPreApplyDamageData : public FExternalAttackData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float ComboAttackMultiplier = 1.f;

	UPROPERTY(VisibleAnywhere)
	float AttackTypeMultiplier = 0.f;

	UPROPERTY(EditAnywhere)
	float AbsoluteDamage;

	virtual void CollectSetByCallerData(EExternalAttackDataSource InSource, TMap<FGameplayTag, float>& OutMap) const override;
	virtual void PrintDebug() const override;
};


USTRUCT(BlueprintType)
struct LQ_GAMEPLAYABILITIESSYSTEM_API FPostApplyDamageData : public FExternalAttackData
{
	GENERATED_BODY()

	virtual void PrintDebug() const override { return; }
	virtual void ExecutePostApplyDamage(AActor* VictimActor, float AppliedDamage, const FGameplayEffectContextHandle& EffectContext) const { return; }
};


USTRUCT(BlueprintType)
struct FPostApplyDamageDataContainer : public FPostApplyDamageData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FPostApplyDamageData> PostApplyDamageDatas;
	virtual void ExecutePostApplyDamage(AActor* VictimActor, float AppliedDamage, const FGameplayEffectContextHandle& EffectContext) const override;
};

USTRUCT()
struct FLQAbilityPayloadDataBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta=(Categories="LQ.AbilityAttackType"))
	FGameplayTag AbilityTag;

	UPROPERTY(EditAnywhere)
	TMap<FGameplayAttribute, float> AbilityCosts;

	UPROPERTY(EditAnywhere, meta=(ExcludeBaseStruct))
	TInstancedStruct<FExternalAttackData> ExternalAttackData;
};

USTRUCT()
struct FLQCombatAnimationAbilityPayload : public FLQAbilityPayloadDataBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UAnimMontage> Montage;
};

USTRUCT(BlueprintType)
struct LQ_GAMEPLAYABILITIESSYSTEM_API FLQGameplayAttributeDataTableRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FGameplayAttribute Attribute;

	UPROPERTY(EditAnywhere, meta=(ClampMin="0.0"))
	float Value = 0.f;

	virtual void OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName) override;
};
