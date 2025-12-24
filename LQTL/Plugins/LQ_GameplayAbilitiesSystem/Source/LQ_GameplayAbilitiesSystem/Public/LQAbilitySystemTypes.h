// Copyright Tale Weavers

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "LQAbilitySystemTypes.generated.h"

USTRUCT()
struct FLQAbilityPayloadDataBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FGameplayTag AbilityTag;

	UPROPERTY(EditAnywhere)
	TMap<FGameplayAttribute, float> AbilityCosts;
};

USTRUCT()
struct FLQCombatAnimationAbilityPayload : public FLQAbilityPayloadDataBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UAnimMontage> Montage;
};

USTRUCT(BlueprintType)
struct FLQGameplayAttributeDataTableRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FGameplayAttribute Attribute;
	
	UPROPERTY(EditAnywhere, meta=(ClampMin="0.0"))
	float Value = 0.f;
	
	virtual void OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName) override;
};
