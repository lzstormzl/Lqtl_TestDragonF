// Copyright Tale Weavers

#pragma once

#include "CoreMinimal.h"
#include "LQCharacterAttributeSet.h"
#include "LQPlayerCombatAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class LQ_GAMEPLAYABILITIESSYSTEM_API ULQPlayerCombatAttributeSet : public ULQCharacterAttributeSet
{
	GENERATED_BODY()

protected:
	// Max Dragon Water
	ATTRIBUTE_ACCESSORS_BASIC(ULQPlayerCombatAttributeSet, MaxDragonWater);
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData MaxDragonWater;

	// Max Dragon Water
	ATTRIBUTE_ACCESSORS_BASIC(ULQPlayerCombatAttributeSet, CurrentDragonWater);
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", meta=(HideInDetailsView))
	FGameplayAttributeData CurrentDragonWater;

	// Dragon Water Regen
	ATTRIBUTE_ACCESSORS_BASIC(ULQPlayerCombatAttributeSet, DragonWaterRegen);
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData DragonWaterRegen;

	// BlessingPoint Max
	ATTRIBUTE_ACCESSORS_BASIC(ULQPlayerCombatAttributeSet, MaxBlessingPoint);
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData MaxBlessingPoint;

	// Current Blessing Point
	ATTRIBUTE_ACCESSORS_BASIC(ULQPlayerCombatAttributeSet, CurrentBlessingPoint);
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", meta=(HideInDetailsView))
	FGameplayAttributeData CurrentBlessingPoint;
};
