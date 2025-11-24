// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/NoExportTypes.h"

#include "LQTL_TT_Development/Struct/S_CombatResource.h"
#include "LQTL_TT_Development/Object/Status Effect/StatusEffectBase.h"

#include "HitboxEffects.generated.h"

/*
Base class for all effects applied to a combat hitbox.
*/
UCLASS(Abstract, NotBlueprintable, EditInlineNew)
class LQTL_TT_DEVELOPMENT_API UHitboxEffectBase : public UObject
{
	GENERATED_BODY()

public:
	// Effects with the same ID will only be activated once on each "montage", 
	// or until ResetIDTrack() is called in hitboxHandler
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Settings")
	int32 ID;

	// Apply the effect of this effect to the given target.
	UFUNCTION(BlueprintCallable)
	virtual void ApplyEffect(AActor* _Target) const { }
};

/*
Apply the given status effect when hit.
*/
UCLASS(NotBlueprintable, meta = (DisplayName = "Status Effect"))
class LQTL_TT_DEVELOPMENT_API UHitboxEffectApplyStatusEffect : public UHitboxEffectBase 
{
	GENERATED_BODY()

public:
	// UStatusEffectBase class to apply
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Settings")
	TSubclassOf<UStatusEffectBase> EffectClass;

	// Strength of the effect to apply
	// NOTE: Effects have their own behaviour based on the level applied
	// (i.e. Increased duration, Increased strength of effect, etc.)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Settings")
	float Level;

	virtual void ApplyEffect(AActor* _Target) const override;
};

/*
Apply the given combat resource to when hit.
*/
UCLASS(NotBlueprintable, meta = (DisplayName = "Combat Resource"))
class LQTL_TT_DEVELOPMENT_API UHitboxEffectCombatResource : public UHitboxEffectBase
{
	GENERATED_BODY()

public:
	// Delta CombatResource to apply to target.
	// NOTE: This value will be handled through the default combat unit system.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Settings")
	FS_CombatResource CombatResource;

	virtual void ApplyEffect(AActor* _Target) const override;
};