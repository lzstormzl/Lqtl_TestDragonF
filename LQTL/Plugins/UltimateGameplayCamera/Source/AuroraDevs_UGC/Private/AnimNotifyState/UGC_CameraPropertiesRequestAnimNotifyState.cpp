// Copyright(c) Aurora Devs 2022-2025. All Rights Reserved.

#include "AnimNotifyState/UGC_CameraPropertiesRequestAnimNotifyState.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Camera/Modifiers/UGC_CameraPropertiesAnimNotifyModifiers.h"
#include "Camera/UGC_PlayerCameraManager.h"

FLinearColor UUGC_FOVRequestAnimNotifyState::GetEditorColor()
{
#if WITH_EDITORONLY_DATA
	const uint8 Ratio = 255 - static_cast<uint8>(FMath::GetMappedRangeValueClamped(FVector2D(5.f, 170.f), FVector2D(0.f, 255.f), TargetFOV));
	NotifyColor = FColor(255, Ratio, Ratio, 255);
	return NotifyColor;
#else
	return FLinearColor::Black;
#endif
}

void UUGC_FOVRequestAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	AActor* Owner = MeshComp->GetOwner();
	if (APawn* OwnerPawn = Cast<APawn>(Owner))
	{
		if (APlayerController* PC = OwnerPawn->GetController<APlayerController>())
		{
			if (AUGC_PlayerCameraManager* CameraManager = Cast<AUGC_PlayerCameraManager>(PC->PlayerCameraManager))
			{
				if (UUGC_FOVAnimNotifyCameraModifier* Modifier = CameraManager->FindCameraModifierOfType<UUGC_FOVAnimNotifyCameraModifier>())
				{
					Modifier->PushFOVAnimNotifyRequest(RequestId, TargetFOV, TotalDuration, BlendInDuration, BlendInCurve, BlendOutDuration, BlendOutCurve);
				}
			}
		}
	}
}

FString UUGC_FOVRequestAnimNotifyState::GetNotifyName_Implementation() const
{
#if WITH_EDITORONLY_DATA
	if (bShowRequestIdInName)
	{
		return FString::Printf(TEXT("[%s][UGC] FOV Request: %.2f"), *RequestId.ToString(), TargetFOV);
	}
	else
#endif
	{
		return FString::Printf(TEXT("[UGC] FOV Request: %.2f"), TargetFOV);
	}
}

FLinearColor UUGC_ArmOffsetRequestAnimNotifyState::GetEditorColor()
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(255, 212, 105, 255);
	return NotifyColor;
#else
	return FLinearColor::Black;
#endif
}

void UUGC_ArmOffsetRequestAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	AActor* Owner = MeshComp->GetOwner();
	if (APawn* OwnerPawn = Cast<APawn>(Owner))
	{
		if (APlayerController* PC = OwnerPawn->GetController<APlayerController>())
		{
			if (AUGC_PlayerCameraManager* CameraManager = Cast<AUGC_PlayerCameraManager>(PC->PlayerCameraManager))
			{
				if (UUGC_ArmOffsetAnimNotifyCameraModifier* Modifier = CameraManager->FindCameraModifierOfType<UUGC_ArmOffsetAnimNotifyCameraModifier>())
				{
					if (bModifySocketOffset)
					{
						Modifier->PushArmSocketOffsetAnimNotifyRequest(RequestId, TargetSocketOffset, TotalDuration, SocketOffsetBlendInDuration, SocketOffsetBlendInCurve, SocketOffsetBlendOutDuration, SocketOffsetBlendOutCurve);
					}

					if (bModifyTargetOffset)
					{
						Modifier->PushArmTargetOffsetAnimNotifyRequest(RequestId, TargetTargetOffset, TotalDuration, TargetOffsetBlendInDuration, TargetOffsetBlendInCurve, TargetOffsetBlendOutDuration, TargetOffsetBlendOutCurve);
					}
				}
			}
		}
	}
}

FString UUGC_ArmOffsetRequestAnimNotifyState::GetNotifyName_Implementation() const
{
#if WITH_EDITORONLY_DATA
	if (bShowRequestIdInName)
	{
		FString Text = FString::Printf(TEXT("[%s][UGC]"), *RequestId.ToString());
		if (bModifySocketOffset)
		{
			Text.Append(FString::Printf(TEXT(" Socket: %s"), *TargetSocketOffset.ToCompactString()));
		}
		if (bModifyTargetOffset)
		{
			Text.Append(FString::Printf(TEXT(" Target: %s"), *TargetTargetOffset.ToCompactString()));
		}
		return Text;
	}
	else
#endif
	{
		FString Text = TEXT("[UGC]");
		if (bModifySocketOffset)
		{
			Text.Append(FString::Printf(TEXT(" Socket: %s"), *TargetSocketOffset.ToCompactString()));
		}
		if (bModifyTargetOffset)
		{
			Text.Append(FString::Printf(TEXT(" Target: %s"), *TargetTargetOffset.ToCompactString()));
		}
		return Text;
	}
}

FLinearColor UUGC_ArmLengthRequestAnimNotifyState::GetEditorColor()
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(55, 219, 33, 255);
	return NotifyColor;
#else
	return FLinearColor::Black;
#endif
}

void UUGC_ArmLengthRequestAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	AActor* Owner = MeshComp->GetOwner();
	if (APawn* OwnerPawn = Cast<APawn>(Owner))
	{
		if (APlayerController* PC = OwnerPawn->GetController<APlayerController>())
		{
			if (AUGC_PlayerCameraManager* CameraManager = Cast<AUGC_PlayerCameraManager>(PC->PlayerCameraManager))
			{
				if (UUGC_ArmLengthAnimNotifyCameraModifier* Modifier = CameraManager->FindCameraModifierOfType<UUGC_ArmLengthAnimNotifyCameraModifier>())
				{
					Modifier->PushArmLengthAnimNotifyRequest(RequestId, TargetArmLength, TotalDuration, BlendInDuration, BlendInCurve, BlendOutDuration, BlendOutCurve);
				}
			}
		}
	}
}

FString UUGC_ArmLengthRequestAnimNotifyState::GetNotifyName_Implementation() const
{
#if WITH_EDITORONLY_DATA
	if (bShowRequestIdInName)
	{
		return FString::Printf(TEXT("[%s][UGC] Arm Length: %.2f"), *RequestId.ToString(), TargetArmLength);
	}
	else
#endif
	{
		return FString::Printf(TEXT("[UGC] Arm Length: %.2f"), TargetArmLength);
	}
}