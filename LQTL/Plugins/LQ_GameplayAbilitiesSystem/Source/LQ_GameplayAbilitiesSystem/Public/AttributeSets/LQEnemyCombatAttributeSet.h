// Copyright Tale Weavers

#pragma once

#include "CoreMinimal.h"
#include "LQCharacterAttributeSet.h"
#include "LQEnemyCombatAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class LQ_GAMEPLAYABILITIESSYSTEM_API ULQEnemyCombatAttributeSet : public ULQCharacterAttributeSet
{
	GENERATED_BODY()

public:
	ATTRIBUTE_ACCESSORS_BASIC(ULQEnemyCombatAttributeSet, MaxSpiritOrb)
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData MaxSpiritOrb;

	ATTRIBUTE_ACCESSORS_BASIC(ULQEnemyCombatAttributeSet, CurrentSpiritOrb)
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", meta=(HideInDetailsView))
	FGameplayAttributeData CurrentSpiritOrb;

	ATTRIBUTE_ACCESSORS_BASIC(ULQEnemyCombatAttributeSet, WaterAttackPower)
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData WaterAttackPower;
};
