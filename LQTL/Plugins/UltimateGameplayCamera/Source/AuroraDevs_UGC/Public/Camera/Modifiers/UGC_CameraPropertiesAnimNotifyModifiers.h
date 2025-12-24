// Copyright(c) Aurora Devs 2022-2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Camera/Modifiers/UGC_CameraModifier.h"
#include "Misc/Build.h"
#include "Misc/Guid.h"
#include "UGC_CameraPropertyRequestStackHelper.h"
#include "UGC_CameraPropertiesAnimNotifyModifiers.generated.h"

/**
 * Camera Modifier in charge of handling FOV change requests from Anim Notifies.
 */
UCLASS(abstract, ClassGroup = "UGC Camera Modifiers")
class AURORADEVS_UGC_API UUGC_FOVAnimNotifyCameraModifier : public UUGC_CameraModifier
{
	GENERATED_BODY()

public:
	UUGC_FOVAnimNotifyCameraModifier();

protected:
	void PushFOVAnimNotifyRequest(FGuid RequestId, float TargetFOV, float TotalDuration, float BlendInDuration, UCurveFloat* BlendInCurve, float BlendOutDuration, UCurveFloat* BlendOutCurve);
	void PopFOVAnimNotifyRequest(FGuid RequestId);

protected:
	void ProcessBoomLengthAndFOV_Implementation(float DeltaTime, float InFOV, float InArmLength, FVector ViewLocation, FRotator ViewRotation, float& OutFOV, float& OutArmLength) override;
	void OnModifierDisabled_Implementation(FMinimalViewInfo const& LastPOV, bool bWasImmediate);

protected:
	friend class UUGC_FOVRequestAnimNotifyState;
	UGC_CameraPropertyRequestStackHelper<float> RequestHelper;
};

/**
 * Camera Modifier in charge of handling Arm Offset changes requests from Anim Notifies.
 */
UCLASS(abstract, ClassGroup = "UGC Camera Modifiers")
class AURORADEVS_UGC_API UUGC_ArmOffsetAnimNotifyCameraModifier : public UUGC_CameraModifier
{
	GENERATED_BODY()

public:
	UUGC_ArmOffsetAnimNotifyCameraModifier();

protected:
	void PushArmSocketOffsetAnimNotifyRequest(FGuid RequestId, FVector TargetOffset, float TotalDuration, float BlendInDuration, UCurveFloat* BlendInCurve, float BlendOutDuration, UCurveFloat* BlendOutCurve);
	void PopArmSocketOffsetAnimNotifyRequest(FGuid RequestId);

	void PushArmTargetOffsetAnimNotifyRequest(FGuid RequestId, FVector TargetOffset, float TotalDuration, float BlendInDuration, UCurveFloat* BlendInCurve, float BlendOutDuration, UCurveFloat* BlendOutCurve);
	void PopArmTargetOffsetAnimNotifyRequest(FGuid RequestId);

protected:
	void ProcessBoomOffsets_Implementation(float DeltaTime, FVector InSocketOffset, FVector InTargetOffset, FVector ViewLocation, FRotator ViewRotation, FVector& OutSocketOffset, FVector& OutTargetOffset) override;
	void OnModifierDisabled_Implementation(FMinimalViewInfo const& LastPOV, bool bWasImmediate);

protected:
	friend class UUGC_ArmOffsetRequestAnimNotifyState;
	UGC_CameraPropertyRequestStackHelper<FVector> SocketOffsetRequestHelper;
	UGC_CameraPropertyRequestStackHelper<FVector> TargetOffsetRequestHelper;
};

/**
 * Camera Modifier in charge of handling Arm Length changes requests from Anim Notifies.
 */
UCLASS(abstract, ClassGroup = "UGC Camera Modifiers")
class AURORADEVS_UGC_API UUGC_ArmLengthAnimNotifyCameraModifier : public UUGC_CameraModifier
{
	GENERATED_BODY()

public:
	UUGC_ArmLengthAnimNotifyCameraModifier();

protected:
	void PushArmLengthAnimNotifyRequest(FGuid RequestId, float TargetLength, float TotalDuration, float BlendInDuration, UCurveFloat* BlendInCurve, float BlendOutDuration, UCurveFloat* BlendOutCurve);
	void PopArmLengthAnimNotifyRequest(FGuid RequestId);

protected:
	void ProcessBoomLengthAndFOV_Implementation(float DeltaTime, float InFOV, float InArmLength, FVector ViewLocation, FRotator ViewRotation, float& OutFOV, float& OutArmLength) override;
	void OnModifierDisabled_Implementation(FMinimalViewInfo const& LastPOV, bool bWasImmediate);

protected:
	friend class UUGC_ArmLengthRequestAnimNotifyState;
	UGC_CameraPropertyRequestStackHelper<float> RequestHelper;
};