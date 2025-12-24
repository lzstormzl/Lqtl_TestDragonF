// Copyright(c) Aurora Devs 2022-2025. All Rights Reserved.

#include "Camera/Modifiers/UGC_CameraPropertiesAnimNotifyModifiers.h"

#include "Curves/CurveFloat.h"
#include "Engine/Engine.h"

UUGC_FOVAnimNotifyCameraModifier::UUGC_FOVAnimNotifyCameraModifier()
	: Super()
{
	bPlayDuringCameraAnimations = true;
	RequestHelper.Init(TEXT("FOV"), [this]() -> bool { return bDebug; });
}

void UUGC_FOVAnimNotifyCameraModifier::ProcessBoomLengthAndFOV_Implementation(float DeltaTime, float InFOV, float InArmLength, FVector ViewLocation, FRotator ViewRotation, float& OutFOV, float& OutArmLength)
{
	Super::ProcessBoomLengthAndFOV_Implementation(DeltaTime, InFOV, InArmLength, ViewLocation, ViewRotation, OutFOV, OutArmLength);
	RequestHelper.ProcessValue(DeltaTime, InFOV, ViewLocation, ViewRotation, OutFOV);
}

void UUGC_FOVAnimNotifyCameraModifier::OnModifierDisabled_Implementation(FMinimalViewInfo const& LastPOV, bool bWasImmediate)
{
	Super::OnModifierDisabled_Implementation(LastPOV, bWasImmediate);
	RequestHelper.OnModifierDisabled(LastPOV, bWasImmediate);
}

void UUGC_FOVAnimNotifyCameraModifier::PushFOVAnimNotifyRequest(FGuid RequestId, float TargetFOV, float TotalDuration, float BlendInDuration, UCurveFloat* BlendInCurve, float BlendOutDuration, UCurveFloat* BlendOutCurve)
{
	RequestHelper.PushValueRequest(RequestId, TargetFOV, TotalDuration, BlendInDuration, BlendInCurve, BlendOutDuration, BlendOutCurve);
}

void UUGC_FOVAnimNotifyCameraModifier::PopFOVAnimNotifyRequest(FGuid RequestId)
{
	RequestHelper.PopValueRequest(RequestId);
}

UUGC_ArmOffsetAnimNotifyCameraModifier::UUGC_ArmOffsetAnimNotifyCameraModifier()
	: Super()
{
	bPlayDuringCameraAnimations = false;
	SocketOffsetRequestHelper.Init(TEXT("SocketOffset"), [this]() -> bool { return bDebug; });
	TargetOffsetRequestHelper.Init(TEXT("TargetOffset"), [this]() -> bool { return bDebug; });

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	SocketOffsetRequestHelper.PropertyColor = FColor(252, 195, 53, 255);
	TargetOffsetRequestHelper.PropertyColor = FColor(255, 212, 105, 255);
#endif
}

void UUGC_ArmOffsetAnimNotifyCameraModifier::ProcessBoomOffsets_Implementation(float DeltaTime, FVector InSocketOffset, FVector InTargetOffset, FVector ViewLocation, FRotator ViewRotation, FVector& OutSocketOffset, FVector& OutTargetOffset)
{
	Super::ProcessBoomOffsets_Implementation(DeltaTime, InSocketOffset, InTargetOffset, ViewLocation, ViewRotation, OutSocketOffset, OutTargetOffset);
	SocketOffsetRequestHelper.ProcessValue(DeltaTime, InSocketOffset, ViewLocation, ViewRotation, OutSocketOffset);
	TargetOffsetRequestHelper.ProcessValue(DeltaTime, InTargetOffset, ViewLocation, ViewRotation, OutTargetOffset);
}

void UUGC_ArmOffsetAnimNotifyCameraModifier::OnModifierDisabled_Implementation(FMinimalViewInfo const& LastPOV, bool bWasImmediate)
{
	Super::OnModifierDisabled_Implementation(LastPOV, bWasImmediate);
	SocketOffsetRequestHelper.OnModifierDisabled(LastPOV, bWasImmediate);
	TargetOffsetRequestHelper.OnModifierDisabled(LastPOV, bWasImmediate);
}

void UUGC_ArmOffsetAnimNotifyCameraModifier::PushArmSocketOffsetAnimNotifyRequest(FGuid RequestId, FVector TargetOffset, float TotalDuration, float BlendInDuration, UCurveFloat* BlendInCurve, float BlendOutDuration, UCurveFloat* BlendOutCurve)
{
	SocketOffsetRequestHelper.PushValueRequest(RequestId, TargetOffset, TotalDuration, BlendInDuration, BlendInCurve, BlendOutDuration, BlendOutCurve);
}

void UUGC_ArmOffsetAnimNotifyCameraModifier::PopArmSocketOffsetAnimNotifyRequest(FGuid RequestId)
{
	SocketOffsetRequestHelper.PopValueRequest(RequestId);
}

void UUGC_ArmOffsetAnimNotifyCameraModifier::PushArmTargetOffsetAnimNotifyRequest(FGuid RequestId, FVector TargetOffset, float TotalDuration, float BlendInDuration, UCurveFloat* BlendInCurve, float BlendOutDuration, UCurveFloat* BlendOutCurve)
{
	TargetOffsetRequestHelper.PushValueRequest(RequestId, TargetOffset, TotalDuration, BlendInDuration, BlendInCurve, BlendOutDuration, BlendOutCurve);
}

void UUGC_ArmOffsetAnimNotifyCameraModifier::PopArmTargetOffsetAnimNotifyRequest(FGuid RequestId)
{
	TargetOffsetRequestHelper.PopValueRequest(RequestId);
}

UUGC_ArmLengthAnimNotifyCameraModifier::UUGC_ArmLengthAnimNotifyCameraModifier()
	: Super()
{
	bPlayDuringCameraAnimations = false;
	RequestHelper.Init(TEXT("ArmLength"), [this]() -> bool { return bDebug; });
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	RequestHelper.PropertyColor = FColor(55, 219, 33, 255);
#endif
}

void UUGC_ArmLengthAnimNotifyCameraModifier::ProcessBoomLengthAndFOV_Implementation(float DeltaTime, float InFOV, float InArmLength, FVector ViewLocation, FRotator ViewRotation, float& OutFOV, float& OutArmLength)
{
	Super::ProcessBoomLengthAndFOV_Implementation(DeltaTime, InFOV, InArmLength, ViewLocation, ViewRotation, OutFOV, OutArmLength);
	RequestHelper.ProcessValue(DeltaTime, InArmLength, ViewLocation, ViewRotation, OutArmLength);
}

void UUGC_ArmLengthAnimNotifyCameraModifier::OnModifierDisabled_Implementation(FMinimalViewInfo const& LastPOV, bool bWasImmediate)
{
	Super::OnModifierDisabled_Implementation(LastPOV, bWasImmediate);
	RequestHelper.OnModifierDisabled(LastPOV, bWasImmediate);
}

void UUGC_ArmLengthAnimNotifyCameraModifier::PushArmLengthAnimNotifyRequest(FGuid RequestId, float TargetLength, float TotalDuration, float BlendInDuration, UCurveFloat* BlendInCurve, float BlendOutDuration, UCurveFloat* BlendOutCurve)
{
	RequestHelper.PushValueRequest(RequestId, TargetLength, TotalDuration, BlendInDuration, BlendInCurve, BlendOutDuration, BlendOutCurve);
}

void UUGC_ArmLengthAnimNotifyCameraModifier::PopArmLengthAnimNotifyRequest(FGuid RequestId)
{
	RequestHelper.PopValueRequest(RequestId);
}