// Copyright(c) Aurora Devs 2022-2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CameraAnimationCameraModifier.h"
#include "DrawDebugHelpers.h"
#include "UGC_CameraAnimationModifier.generated.h"

/**
 * Delegate for when a UGC Camera Animation is completed, whether the animation has been interrupted or finished.
 *
 * bInterrupted = true if it was not property finished
 */
DECLARE_DELEGATE_TwoParams(FOnCameraAnimationEnded, class UCameraAnimationSequence*, bool /*bInterrupted*/)

/**
 * Delegate for when a UGC Camera Animation started easing out, whether the animation has actually been interrupted or not.
 *
 * bInterrupted = true if it was not property finished
 */
DECLARE_DELEGATE_OneParam(FOnCameraAnimationEaseOutStarted, class UCameraAnimationSequence*)

UENUM()
enum class ECameraAnimationResetType : uint8
{
	BackToStart UMETA(ToolTip = "The camera will go back to the position it started from."),
	ResetToZero UMETA(ToolTip = "The camera's orientation will be reset to zero. This is usually the back of the character. If UseControllerRotationYaw is true, this is forcefully used."),
	ContinueFromEnd UMETA(ToolTip = "The camera will blend out from the last position of the animation.")
};

USTRUCT()
struct FUGCActiveAnimationInfo
{
	GENERATED_USTRUCT_BODY()

	ECameraAnimationResetType ResetType = ECameraAnimationResetType::ResetToZero;
	/** Runtime interpolated distance percentage (0 = fully blocked, 1 = clear) */
	float DistBlockedPct = 1.f;
	bool bDoCollisionChecks = false;
	bool bWasEasingOut = false;
};

/**
 * Gameplay Camera Animation Modifier which plays in the correct transform space in rgeards to the owning player.
 */
UCLASS(abstract, ClassGroup = "UGC Camera Modifiers")
class AURORADEVS_UGC_API UUGC_CameraAnimationModifier : public UCameraAnimationCameraModifier
{
	GENERATED_BODY()

public:
	// UCameraModifier interface
	virtual bool ModifyCamera(float DeltaTime, FMinimalViewInfo& InOutPOV) override;

	void CameraAnimation_SetEasingOutDelegate(FOnCameraAnimationEaseOutStarted& InOnAnimationEaseOutStarted, FCameraAnimationHandle AnimationHandle);
	void CameraAnimation_SetEndedDelegate(FOnCameraAnimationEnded& InOnAnimationEnded, FCameraAnimationHandle AnimationHandle);

	FCameraAnimationHandle PlaySingleCameraAnimation(UCameraAnimationSequence* Sequence, FCameraAnimationParams Params, ECameraAnimationResetType ResetType, bool bInterruptOthers, bool bDoCollisionChecks);

	/**
	 * Stops the given camera animation sequence. If nullptr, will stop whatever is currently active.
	 * @param Sequence        The camera sequence animation.
	 * @param bImmediate	True to stop it right now and ignore blend out, false to let it blend out as indicated.
	 */
	UFUNCTION(BlueprintCallable, Category = "UGC|Modifiers|Camera Animation")
	void StopCameraAnimationSequence(UCameraAnimationSequence* Sequence, bool bImmediate = false);

	/**
	 * Get the current camera animation playing on this modifier.
	 * @return      The current camera animation playing.
	 */
	UFUNCTION(BlueprintCallable, Category = "UGC|Modifiers|Camera Animations")
	void GetCurrentCameraAnimations(TArray<UCameraAnimationSequence*>& OutAnimations) const;

	/**
	 * Returns whether the given camera animation is playing on this modifier.
	 * @param Sequence      The Camera Animation Sequence.
	 * @return              Whether the corresponding camera animation is playing or not.
	 */
	UFUNCTION(BlueprintCallable, Category = "UGC|Modifiers|Camera Animations")
	bool IsCameraAnimationSequenceActive(UCameraAnimationSequence* Sequence) const;

	/**
	 * Returns whether any camera animation is playing on this modifier.
	 * @param Sequence      The Camera Animation Sequence.
	 * @return              Whether any camera animation is playing or not.
	 */
	UFUNCTION(BlueprintCallable, Category = "UGC|Modifiers|Camera Animations")
	bool IsAnyCameraAnimationSequence() const;

protected:
	void UGCDeactivateCameraAnimation(FActiveCameraAnimationInfo& ActiveAnimation);

	// UCameraAnimationCameraModifier interface
	virtual void UGCTickActiveAnimation(float DeltaTime, FMinimalViewInfo& InOutPOV);

	// UCameraAnimationCameraModifier interface
	virtual void UGCTickAnimation(FActiveCameraAnimationInfo& CameraAnimation, float DeltaTime, FMinimalViewInfo& InOutPOV, int Index);
	virtual void UGCTickAnimCollision(FActiveCameraAnimationInfo& CameraAnimation, float DeltaTime, FMinimalViewInfo& InOutPOV, int Index);

	FVector GetTraceSafeLocation(FMinimalViewInfo const& InPOV);

	template<typename T>
	T* GetViewTargetAs() const
	{
		return Cast<T>(GetViewTarget());
	}

private:
#if ENABLE_DRAW_DEBUG
	void UGCDebugAnimation(FActiveCameraAnimationInfo& ActiveAnimation, float DeltaTime);
#endif

private:
	UPROPERTY(Transient)
	TObjectPtr<class AUGC_PlayerCameraManager> UGCCameraManager = nullptr;
	UPROPERTY(Transient)
	TObjectPtr<class UUGC_CameraCollisionModifier> CollisionModifier = nullptr;

	/** How should the camera behave after the current animation is over. */
	UPROPERTY(Transient)
	TArray<FUGCActiveAnimationInfo> UGCAnimInfo;

	UPROPERTY(Transient)
	int LastIndex = 0;

	// Delegates
	FOnCameraAnimationEnded OnAnimationEnded;
	FOnCameraAnimationEaseOutStarted OnAnimationEaseOutStarted;
};