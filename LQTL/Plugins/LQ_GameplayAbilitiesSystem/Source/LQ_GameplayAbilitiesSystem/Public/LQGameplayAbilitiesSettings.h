// Copyright Tale Weavers

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DeveloperSettings.h"
#include "LQGameplayAbilitiesSettings.generated.h"

class UGameplayEffect;
class UCurveFloat;

/**
 *
 */
UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="LQ Gameplay Abilities"))
class LQ_GAMEPLAYABILITIESSYSTEM_API ULQGameplayAbilitiesSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	static const ULQGameplayAbilitiesSettings* Get();

	UPROPERTY(EditAnywhere, Config)
	TSubclassOf<UGameplayEffect> GE_DamageInstance;

	// Defense to damage reduction curve (X = Defense value, Y = Damage reduction percentage 0-1)
	UPROPERTY(EditAnywhere, Config, Category = "Damage")
	TSoftObjectPtr<UCurveFloat> DefenseToReductionCurve;

	UPROPERTY(EditAnywhere, Config, Category = "Damage", meta=(Categories="LQ.AbilityAttackType"))
	TMap<FGameplayTag, float> AttackTypeDamageMutipliers;
};
