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

public:
	// Attribute accessors (public for damage calculation and external access)
	ATTRIBUTE_ACCESSORS_BASIC(ULQCharacterAttributeSet, WalkSpeed);
	ATTRIBUTE_ACCESSORS_BASIC(ULQCharacterAttributeSet, SprintSpeed);
	ATTRIBUTE_ACCESSORS_BASIC(ULQCharacterAttributeSet, TurnSpeed);
	ATTRIBUTE_ACCESSORS_BASIC(ULQCharacterAttributeSet, MaxHP);
	ATTRIBUTE_ACCESSORS_BASIC(ULQCharacterAttributeSet, CurrentHP);
	ATTRIBUTE_ACCESSORS_BASIC(ULQCharacterAttributeSet, MaxShield);
	ATTRIBUTE_ACCESSORS_BASIC(ULQCharacterAttributeSet, Defense);
	ATTRIBUTE_ACCESSORS_BASIC(ULQCharacterAttributeSet, AttackPower);
	ATTRIBUTE_ACCESSORS_BASIC(ULQCharacterAttributeSet, SpiritAttackPower);
	ATTRIBUTE_ACCESSORS_BASIC(ULQCharacterAttributeSet, StaggerResistance);
	ATTRIBUTE_ACCESSORS_BASIC(ULQCharacterAttributeSet, CritRate);
	ATTRIBUTE_ACCESSORS_BASIC(ULQCharacterAttributeSet, CritDamage);
	ATTRIBUTE_ACCESSORS_BASIC(ULQCharacterAttributeSet, StatusResistance);
	ATTRIBUTE_ACCESSORS_BASIC(ULQCharacterAttributeSet, DamageMultiplier);

protected:
	virtual void InitFromMetaDataTable(const UDataTable* DataTable) override;
	virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData WalkSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData SprintSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData TurnSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData MaxHP;

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData CurrentHP;

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData MaxShield;

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Defense;

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData AttackPower;

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData SpiritAttackPower;

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData StaggerResistance;

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData CritRate;

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData CritDamage;

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData StatusResistance;

	// Damage Multiplier (additive: 0 = no change, 0.2 = +20% damage)
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData DamageMultiplier;
};
