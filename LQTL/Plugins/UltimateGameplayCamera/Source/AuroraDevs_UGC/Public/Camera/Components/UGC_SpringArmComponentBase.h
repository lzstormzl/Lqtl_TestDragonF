// Copyright(c) Aurora Devs 2022-2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/Data/UGC_CameraData.h"
#include "Misc/CoreMiscDefines.h"
#include "UGC_SpringArmComponentBase.generated.h"

/**
 * Custom SpringArm component with enhanced collision handling.
 */
UCLASS(Blueprintable, Abstract, ClassGroup = "UGC Camera", Category = "UGC|Components", meta = (BlueprintSpawnableComponent))
class AURORADEVS_UGC_API UUGC_SpringArmComponentBase : public USpringArmComponent
{
	GENERATED_BODY()

protected:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void UpdateDesiredArmLocation(bool bDoTrace, bool bDoLocationLag, bool bDoRotationLag, float DeltaTime) override;
	virtual FVector BlendLocations(const FVector& DesiredArmLocation, const FVector& TraceHitLocation, bool bHitSomething, float DeltaTime) override;
	bool IsPlayerControlled() const;

protected:
	/** Camera collision settings including feelers */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CameraCollision, meta = (EditCondition = "bDoCollisionTest"))
	FCameraCollisionSettings CameraCollisionSettings;

	/* *EXPERIMENTAL* Might cause bugs.
	 * Whether we want the framing to stay the same during collisions. This is useful for games where you need to aim (bow, gun; etc.) since it
	 * allows the center of the screen to not shift during collision.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CameraCollision)
	bool bMaintainFramingDuringCollisions = false;

	/* Whether to draw debug messages regarding the spring arm collision.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraCollision")
	bool bPrintCollisionDebug = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraCollision", meta = (EditCondition = "bPrintCollisionDebug"))
	bool bPrintHitActors = false;

protected:
	/** Runtime interpolated distance percentage (0 = fully blocked, 1 = clear) */
	float DistBlockedPct = 1.f;

	// Debug-only: Track which actors were hit by feeler rays (not needed in shipping)
#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleInstanceOnly, Transient, Category = "CameraCollision|Debug")
	TArray<class AActor*> HitActors;
#endif
};
