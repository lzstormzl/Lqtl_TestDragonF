// Copyright(c) Aurora Devs 2022-2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "UGC_PlayerCameraManager.generated.h"

/**
 *
 */
UCLASS()
class AURORADEVS_UGC_API AUGC_PlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
public:
	AUGC_PlayerCameraManager();

	virtual void InitializeFor(class APlayerController* PC) override;

	/*
	* This returns the real camera view, this isn't necessarily the view of the camera attached to your character.
	* For example, during camera animations, the camera used is different than the normal gameplay camera.
	*/
	UFUNCTION(BlueprintPure, Category = "UGC|Camera Manager")
	void GetRealCameraView(FVector& OutViewLocation, FRotator& OutViewRotation) { OutViewLocation = ViewTarget.POV.Location; OutViewRotation = ViewTarget.POV.Rotation; }

	/*
	 * Prepare the possession of a new pawn. Doesn't actually possess the pawn.
	 * APlayerController::Possess needs to be called directly after this function.
	 * @param  NewPawn                      The new pawn that we intend on possessing.
	 * @param  NewCameraDA                  The camera data asset that will be pushed after possessing.
											*Requires call to PostPosses after the APlayerController::Possess has been called!*
	 * @param  bBlendSpringArmProperties     Whether the new pawn's spring arm should prepare to blend from the previous spring arm.
											*This doesn't work with SetViewTargetWithBlend!*
											*Requires call to PostPosses after the APlayerController::Possess has been called!*
	 * @param  bMatchCameraRotation         Whether the new pawn should have the same camera spring arm rotation (aka Control Rotation) or not.
											*This doesn't work with SetViewTargetWithBlend!*
											*Requires call to PostPosses after the APlayerController::Possess has been called!*
	 */
	UFUNCTION(BlueprintCallable, Category = "UGC|Camera Manager")
	void PrePossess(class APawn* NewPawn, class UUGC_CameraDataAssetBase* NewCameraDA, bool bBlendSpringArmProperties = false, bool bMatchCameraRotation = false);

	/*
	 * Finush the possession of a new pawn. APlayerController::Possess needs to have been called directly before this function.
	 * @param  bReplaceCurrentCameraDA      Whether to push the Camera DA that was passed in the PrePossess function (if any) should
											replace the current camera DA or be pushed on top of it.
	 */
	UFUNCTION(BlueprintCallable, Category = "UGC|Camera Manager")
	void PostPossess(bool bReplaceCurrentCameraDA = true);

	UFUNCTION(BlueprintCallable, Category = "UGC|Camera Manager")
	void RefreshLevelSequences();

	UFUNCTION(BlueprintPure, Category = "UGC|Camera Manager")
	bool IsPlayingAnyLevelSequence() const { return NbrActiveLevelSequences > 0 || NbrActivePausedLevelSequences > 0; }

	// Plays a single new camera animation sequence. Any subsequent calls while this animation runs will interrupt the current animation.
	// This variation can be used in contexts where async nodes aren't allowd (e.g., AnimNotifies).
	UFUNCTION(BlueprintCallable, Category = "UGC|Camera Manager")
	void PlayCameraAnimation(class UCameraAnimationSequence* CameraSequence, struct FUGCCameraAnimationParams const& Params, bool bInterruptOthers, bool bDoCollisionChecks);

	/**
	 *  Enables/Disables all camera modifiers ONLY if they inherit from UGC Camera Modifier.
	 *  @param  bEnabled    - true to enable all UGC camera modifiers, false to disable.
	 *  @param  bImmediate  - If bEnabled is false: true to disable immediately with no blend out, false (default) to allow blend out
	 */
	UFUNCTION(BlueprintCallable, Category = "UGC|Camera Manager")
	void ToggleUGCCameraModifiers(bool const bEnabled, bool const bImmediate = true);

	/**
	 *  Enables/Disables all camera modifiers, regardless of whether they inherit from UGC Camera Modifier.
	 *  @param  bEnabled    - true to enable all camera modifiers, false to disable.
	 *  @param  bImmediate  - If bEnabled is false: true to disable immediately with no blend out, false (default) to allow blend out
	 */
	UFUNCTION(BlueprintCallable, Category = "UGC|Camera Manager")
	void ToggleCameraModifiers(bool const bEnabled, bool const bImmediate = true);

	/**
	 *  Enables/Disables all debugging of camera modifiers ONLY if they inherit from UGC Camera Modifier.
	 *  @param  bEnabled    - true to enable all debugging of UGC camera modifiers, false to disable.
	 */
	UFUNCTION(BlueprintCallable, Category = "UGC|Camera Manager")
	void ToggleAllUGCModifiersDebug(bool const bEnabled);
	/**
	 *
	 *  Enables/Disables all debugging of camera modifiers regardless of whether they inherit from UGC Camera Modifier.
	 *  @param  bEnabled    - true to enable all debugging of all camera modifiers, false to disable.
	 */
	UFUNCTION(BlueprintCallable, Category = "UGC|Camera Manager")
	void ToggleAllModifiersDebug(bool const bEnabled);

	UFUNCTION(BlueprintPure, Category = "UGC|Camera Manager")
	class USpringArmComponent* GetOwnerSpringArmComponent() const { return CameraArm.Get(); }

	UFUNCTION(BlueprintPure, Category = "UGC|Camera Manager")
	class UUGC_CameraDataAssetBase* GetCurrentCameraDataAsset() const { return CameraDataStack.IsEmpty() ? nullptr : CameraDataStack[CameraDataStack.Num() - 1]; }

	UFUNCTION(BlueprintCallable, Category = "UGC|Camera Manager")
	void PushCameraData(class UUGC_CameraDataAssetBase* CameraDA);

	// Pops the most recent Camera DA.
	UFUNCTION(BlueprintCallable, Category = "UGC|Camera Manager")
	void PopCameraDataHead();

	// Pops the given Camera DA. If it isn't in stack, it returns false.
	UFUNCTION(BlueprintCallable, Category = "UGC|Camera Manager")
	void PopCameraData(class UUGC_CameraDataAssetBase* CameraDA);

	UFUNCTION(BlueprintNativeEvent, Category = "UGC|Camera Manager")
	void OnCameraDataStackChanged(class UUGC_CameraDataAssetBase* CameraDA, bool bBlendCameraProperties = true);

	/** Draw a debug camera shape at the real camera's location, which can be different from the character's attached camera.
	 * For example, camera animations use a different camera then the character's attached camera.
	 */
	UFUNCTION(BlueprintCallable, Category = "UGC|Camera Manager|Debug")
	void DrawRealDebugCamera(float Duration, FLinearColor CameraColor = FLinearColor::Red, float Thickness = 1.f) const;

	/** Draw a debug camera shape at the character's attached camera's location, which can be different from the real camera.
	 * For example, camera animations use a different camera then the character's attached camera.
	 */
	UFUNCTION(BlueprintCallable, Category = "UGC|Camera Manager|Debug")
	void DrawGameDebugCamera(float Duration, bool bDrawCamera = true, FLinearColor CameraColor = FLinearColor::Blue, bool bDrawSpringArm = true, FLinearColor SpringArmColor = FLinearColor::Blue, float Thickness = 1.f) const;

	/* Draw Spring Arm.
	*/
	UFUNCTION(BlueprintCallable, Category = "UGC|Camera Manager|Debug")
	void DrawDebugSpringArm(FVector const& CameraLocation, float Duration, FLinearColor SpringArmColor = FLinearColor::Blue, float Thickness = 1.f) const;

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "UGC|Camera Manager")
	bool IsPlayingAnyCameraAnimation() const;

	UFUNCTION(BlueprintPure, Category = "UGC|Camera Manager|Movement")
	FVector GetOwnerVelocity() const;

	UFUNCTION(BlueprintPure, Category = "UGC|Camera Manager|Movement")
	void ComputeOwnerFloorDist(float SweepDistance, float CapsuleRadius, bool& bOutFloorExists, float& OutFloorDistance) const;

	UFUNCTION(BlueprintPure, Category = "UGC|Camera Manager|Movement")
	bool IsOwnerFalling() const;

	UFUNCTION(BlueprintPure, Category = "UGC|Camera Manager|Movement")
	bool IsOwnerStrafing() const;

	UFUNCTION(BlueprintPure, Category = "UGC|Camera Manager|Movement")
	bool IsOwnerMovingOnGround() const;

	UFUNCTION(BlueprintPure, Category = "UGC|Camera Manager|Movement")
	void ComputeOwnerFloorNormal(float SweepDistance, float CapsuleRadius, bool& bOutFloorExists, FVector& OutFloorNormal) const;

	UFUNCTION(BlueprintPure, Category = "UGC|Camera Modifier|Movement")
	void ComputeOwnerSlopeAngle(float& OutSlopePitchDegrees, float& OutSlopeRollDegrees);

	/*
	 * Returns value betwen 1 (the character is looking where they're moving) or -1 (looking in the opposite direction they're moving).
	 * Will return 0 if the character isn't moving.
	 */
	UFUNCTION(BlueprintPure, Category = "UGC|Camera Modifier|Movement")
	float ComputeOwnerLookAndMovementDot();

	/*
	 * Only use this when your pawn has multiple spring arm components and you need to use a different one.
	 * The spring arm is automatically reset to the first found spring arm component on the pawn when we initialize
	 * the camera manager or when we possess a new pawn.
	 */
	UFUNCTION(BlueprintCallable, Category = "UGC|Camera Manager")
	void SetSpringArmComponent(class USpringArmComponent* Arm) { if (Arm) CameraArm = Arm; }

	/**
	 * Returns camera modifier for this camera of the given class, if it exists.
	 * Looks for inherited classes too. If there are multiple modifiers which fit, the first one is returned.
	 */
	UFUNCTION(BlueprintCallable, Category = "UGC|Camera Manager", meta = (DeterminesOutputType = "ModifierClass"))
	UCameraModifier* FindCameraModifierOfClass(TSubclassOf<UCameraModifier> ModifierClass, bool bIncludeInherited);

	FVector GetCameraTurnRate() const { return FVector(YawTurnRate, PitchTurnRate, 0.f); }

	UCameraModifier const* FindCameraModifierOfClass(TSubclassOf<UCameraModifier> ModifierClass, bool bIncludeInherited) const;

	template<typename T>
	T* FindCameraModifierOfType()
	{
		bool constexpr bIncludeInherited = true;
		UCameraModifier* Modifier = FindCameraModifierOfClass(T::StaticClass(), bIncludeInherited);
		return Modifier != nullptr ? Cast<T>(Modifier) : nullptr;
	}

	template<typename T>
	T const* FindCameraModifierOfType() const
	{
		bool constexpr bIncludeInherited = true;
		UCameraModifier const* Modifier = FindCameraModifierOfClass(T::StaticClass(), bIncludeInherited);
		return Modifier != nullptr ? Cast<T>(Modifier) : nullptr;
	}
protected:
	void PushCameraData_Internal(class UUGC_CameraDataAssetBase* CameraDA, bool bBlendCameraProperties);

	void DoForEachUGCModifier(TFunction<void(class UUGC_CameraModifier*)> const& Function);

	// Breaks when the function returns true.
	void DoForEachUGCModifierWithBreak(TFunction<bool(class UUGC_CameraModifier*)> const& Function);

	virtual void DisplayDebug(class UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos) override;
	virtual UCameraModifier* AddNewCameraModifier(TSubclassOf<UCameraModifier> ModifierClass) override;
	virtual bool RemoveCameraModifier(UCameraModifier* ModifierToRemove) override;

	virtual void SetViewTarget(AActor* NewViewTarget, FViewTargetTransitionParams TransitionParams) override;

	/**
	 * Called to give PlayerCameraManager a chance to adjust view rotation updates before they are applied.
	 * e.g. The base implementation enforces view rotation limits using LimitViewPitch, et al.
	 * @param DeltaTime - Frame time in seconds.
	 * @param OutViewRotation - In/out. The view rotation to modify.
	 * @param OutDeltaRot - In/out. How much the rotation changed this frame.
	 */
	virtual void ProcessViewRotation(float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot);

	/**
	 * Called to give PlayerCameraManager a chance to adjust both the yaw turn rate and pitch turn rate.
	 *
	 * @param DeltaTime - Frame time in seconds.
	 * @param InLocalControlRotation - The difference between the actor rotation and the control rotation.
	 * @param OutPitchTurnRate - Out. New value of the pitch turn rate (between 0 and 1).
	 * @param OutYawTurnRate - Out. New value of the yaw turn rate (between 0 and 1).
	 * @return Return true to prevent subsequent (lower priority) modifiers to further adjust rotation, false otherwise.
	 */
	virtual void ProcessTurnRate(float DeltaTime, FRotator InLocalControlRotation, float& OutPitchTurnRate, float& OutYawTurnRate);

	virtual void Tick(float DeltaTime) override;

	virtual void LimitViewYaw(FRotator& ViewRotation, float InViewYawMin, float InViewYawMax) override;

	// This updates the internal variables of the UGC Player Camera Manager. Make sure to call the parent function if you override this in BP.
	UFUNCTION(BlueprintNativeEvent, Category = "UGC|Camera Manager|Internal")
	void UpdateInternalVariables(float DeltaTime);

	// Usually uses the UGC Pawn Interface to fetch the rotation input of the camera (Mouse or Right Thumbstick). Override this if you want to provide your own way of getting the camera rotation input.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "UGC|Camera Manager|Internal")
	FRotator GetRotationInput() const;

	// Usually uses the UGC Pawn Interface to fetch the movement input of the character (WASD or Left Thumbstick). Override this if you want to provide your own way of getting the camera rotation input.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "UGC|Camera Manager|Internal")
	FVector GetMovementControlInput() const;

	UFUNCTION()
	void OnLevelSequenceStarted();

	UFUNCTION()
	void OnLevelSequencePaused();

	UFUNCTION()
	void OnLevelSequenceEnded();
protected:
	friend class UUGC_CameraModifier;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "UGC|Camera Manager|Internal")
	TArray<class UUGC_CameraDataAssetBase*> CameraDataStack;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "UGC|Camera Manager|Internal")
	TObjectPtr<class ACharacter> OwnerCharacter;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "UGC|Camera Manager|Internal")
	TObjectPtr<class APawn> OwnerPawn;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "UGC|Camera Manager|Internal")
	TObjectPtr<class USpringArmComponent> CameraArm;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "UGC|Camera Manager|Internal")
	TObjectPtr<class UCharacterMovementComponent> MovementComponent;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "UGC|Camera Manager|Internal")
	TArray<class ALevelSequenceActor*> LevelSequences;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UGC|Camera Manager|Angle Constraints", meta = (UIMin = 0.f, UIMax = 1.f, ClampMin = 0.f, ClampMax = 1.f))
	float PitchTurnRate = 1.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UGC|Camera Manager|Angle Constraints", meta = (UIMin = 0.f, UIMax = 1.f, ClampMin = 0.f, ClampMax = 1.f))
	float YawTurnRate = 1.f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "UGC|Camera Manager|Internal")
	float AspectRatio = 0.f;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "UGC|Camera Manager|Internal")
	float VerticalFOV = 0.f;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "UGC|Camera Manager|Internal")
	float HorizontalFOV = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UGC|Camera Manager|Internal")
	bool bHasMovementInput = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UGC|Camera Manager|Internal")
	FVector MovementInput = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UGC|Camera Manager|Internal")
	float TimeSinceMovementInput = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UGC|Camera Manager|Internal")
	bool bHasRotationInput = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UGC|Camera Manager|Internal")
	FRotator RotationInput = FRotator::ZeroRotator;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UGC|Camera Manager|Internal")
	float TimeSinceRotationInput = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UGC|Camera Manager|Internal")
	float OriginalArmLength = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UGC|Camera Manager|Internal")
	TArray<class UUGC_CameraModifier*> UGCModifiersList;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UGC|Camera Manager|Internal")
	TArray<class UUGC_CameraAddOnModifier*> UGCAddOnModifiersList;

	UPROPERTY(Transient)
	int NbrActiveLevelSequences = 0;

	UPROPERTY(Transient)
	int NbrActivePausedLevelSequences = 0;

	struct PossessPayload
	{
		TObjectPtr<class UUGC_CameraDataAssetBase> PendingCameraDA = nullptr;
		FRotator PendingControlRotation = FRotator::ZeroRotator;
		bool bBlendCameraProperties = false;
		bool bMatchCameraRotation = false;
	} PendingPossessPayload;
};