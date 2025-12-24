// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "LQCharacterAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class LQ_GAMEPLAYABILITIESSYSTEM_API ULQCharacterAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

protected:
	virtual void InitFromMetaDataTable(const UDataTable* DataTable) override;
	
	ATTRIBUTE_ACCESSORS_BASIC(ULQCharacterAttributeSet, WalkSpeed);
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData WalkSpeed;

	ATTRIBUTE_ACCESSORS_BASIC(ULQCharacterAttributeSet, SprintSpeed);
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData SprintSpeed;

	ATTRIBUTE_ACCESSORS_BASIC(ULQCharacterAttributeSet, TurnSpeed);
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData TurnSpeed;

	ATTRIBUTE_ACCESSORS_BASIC(ULQCharacterAttributeSet, MaxHP);
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData MaxHP;

	ATTRIBUTE_ACCESSORS_BASIC(ULQCharacterAttributeSet, CurrentHP);
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", meta=(HideInDetailsView))
	FGameplayAttributeData CurrentHP;

	ATTRIBUTE_ACCESSORS_BASIC(ULQCharacterAttributeSet, MaxShield);
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData MaxShield;

	// Defense
	ATTRIBUTE_ACCESSORS_BASIC(ULQCharacterAttributeSet, Defense);
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Defense;

	// Attack Power
	ATTRIBUTE_ACCESSORS_BASIC(ULQCharacterAttributeSet, AttackPower);
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData AttackPower;

	// Spirit Attack Power
	ATTRIBUTE_ACCESSORS_BASIC(ULQCharacterAttributeSet, SpiritAttackPower);
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData SpiritAttackPower;

	ATTRIBUTE_ACCESSORS_BASIC(ULQCharacterAttributeSet, StaggerResistance);
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData StaggerResistance;

	// Crit Rate
	ATTRIBUTE_ACCESSORS_BASIC(ULQCharacterAttributeSet, CritRate);
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData CritRate;

	// Crit Damage
	ATTRIBUTE_ACCESSORS_BASIC(ULQCharacterAttributeSet, CritDamage);
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData CritDamage;

	// Status Resistance
	ATTRIBUTE_ACCESSORS_BASIC(ULQCharacterAttributeSet, StatusResistance);
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData StatusResistance;
};
