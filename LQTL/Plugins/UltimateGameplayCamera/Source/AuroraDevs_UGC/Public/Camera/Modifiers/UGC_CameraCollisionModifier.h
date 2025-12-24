// Copyright(c) Aurora Devs 2022-2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "UGC_CameraModifier.h"
#include "Camera/Data/UGC_CameraData.h"
#include "Camera/CameraTypes.h"
#include "DrawDebugHelpers.h"
#include "Containers/StaticBitArray.h"
#include "UGC_CameraCollisionModifier.generated.h"

/**
 * DEPRECATED. USE UGC_SpringArmComponent INSTEAD.
 * Camera Modifier which does camera avoidance using predictive collision feelers.
 */
UCLASS(abstract, ClassGroup = "UGC Camera Modifiers", meta = (Deprecated = 5.5))
class AURORADEVS_UGC_API UUGC_CameraCollisionModifier : public UUGC_CameraModifier
{
	GENERATED_BODY()

	UUGC_CameraCollisionModifier();

public:
	// Force collision modifier to use a single ray by another modifier. Do not use this if you're not familiar with it.
	UFUNCTION(BlueprintCallable, Category = "UGC|Modifiers|Collision")
	void AddSingleRayOverrider(UCameraModifier const* OverridingModifier) { if (OverridingModifier) SingleRayOverriders.AddUnique(OverridingModifier); }

	// Remove single ray modifier override. Do not use this if you're not familiar with it.
	UFUNCTION(BlueprintCallable, Category = "UGC|Modifiers|Collision")
	void RemoveSingleRayOverrider(UCameraModifier const* OverridingModifier) { if (OverridingModifier) SingleRayOverriders.Remove(OverridingModifier); }

protected:
	virtual bool ModifyCamera(float DeltaTime, FMinimalViewInfo& InOutPOV) override;

	void UpdatePreventPenetration(float DeltaTime, FMinimalViewInfo& InOutPOV);

	void PreventCameraPenetration(class AActor const& ViewTarget, FVector const& SafeLoc, FVector& OutCameraLoc, float const& DeltaTime, float& OutDistBlockedPct, bool bSingleRayOnly);

	FVector GetTraceSafeLocation(FMinimalViewInfo const& POV);

	void ResetSingleRayOverriders() { SingleRayOverriders.Reset(); }

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UGC|Modifiers|Collision")
	FCameraCollisionSettings CollisionSettings;

	// If you don't want the camera to start close to the character and smoothly pan out once your character is spawned, default-initialize this variable to 1.f.
	UPROPERTY(Transient)
	float AimLineToDesiredPosBlockedPct = 1.f;

	UPROPERTY(Transient)
	TArray<TObjectPtr<const AActor>> DebugActorsHitDuringCameraPenetration;

protected:
	TArray<UCameraModifier const*> SingleRayOverriders;
	TStaticBitArray<128u> CollidingFeelers;

#if ENABLE_DRAW_DEBUG
	mutable float LastDrawDebugTime = -MAX_FLT;
#endif
};