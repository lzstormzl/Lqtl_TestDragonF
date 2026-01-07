// Copyright Tale Weavers


#include "LQHitboxAnimNotifyState.h"

#include "AbilitySystemGlobals.h"
#include "GameplayAbilitySpecHandle.h"
#include "LQAbilitySystemLibrary.h"
#include "LQHitboxManager.h"
#include "StructUtils/StructView.h"

void ULQHitboxAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
                                           const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (MeshComp)
	{
#if WITH_EDITOR
		if (CurrentSockets.IsEmpty())
		{
			CurrentSockets = MeshComp->GetAllSocketNames();
			TArray<USceneComponent*> OutChildrenComps;
			MeshComp->GetChildrenComponents(false, OutChildrenComps);
			for (USceneComponent* Comp : OutChildrenComps)
			{
				if (Comp && (Comp->IsA(UStaticMeshComponent::StaticClass()) || Comp->IsA(USkeletalMeshComponent::StaticClass())))
				{
					CurrentSockets.Append(Comp->GetAllSocketNames());
				}
			}
		}

		if (auto Ptr = AttackData.GetMutablePtr<FHitboxAttackData>())
		{
			if (auto PreApplyPtr = Ptr->PreApplyDamageData.GetMutablePtr<FPreApplyDamageData>())
			{
				PreApplyPtr->MyDataSource = EExternalAttackDataSource::EADS_MontageHitbox;
			}
			if (auto PostApplyPtr = Ptr->PostApplyDamageData.GetMutablePtr<FPostApplyDamageData>())
			{
				PostApplyPtr->MyDataSource = EExternalAttackDataSource::EADS_MontageHitbox;
			}
		}
#endif

		HitboxManager = MeshComp->GetOwner()->FindComponentByClass<ULQHitboxManager>();
		if (HitboxShape.IsValid())
		{
			HitboxShape.GetMutablePtr<FHitboxShapeBase>()->BeginTrace(MeshComp, Animation, TotalDuration);
		}
	}
}

void ULQHitboxAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                         const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	AlreadyHitActors.Empty();
}

void ULQHitboxAnimNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime,
                                          const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	if (MeshComp)
	{
		FHitResult HitResult;
		if (HitboxShape.IsValid())
		{
			if (HitboxShape.Get<FHitboxShapeBase>().TickTrace(MeshComp->GetOwner(), HitResult))
			{
				auto TryFound = AlreadyHitActors.FindPair(TObjectKey<AActor>(HitResult.GetActor()), TObjectKey<UPrimitiveComponent>(HitResult.GetComponent()));
				if (TryFound == nullptr)
				{
					AlreadyHitActors.Add(TObjectKey<AActor>(HitResult.GetActor()), TObjectKey<UPrimitiveComponent>(HitResult.GetComponent()));
					if (HitboxManager)
					{
						auto AnimDrivenSpecHandle = ULQAbilitySystemLibrary::GetAbilitySpecHandleFromMontage(
							ULQAbilitySystemLibrary::GetLQASCFromActor(MeshComp->GetOwner()), Cast<class UAnimMontage>(Animation));
						if (AttackData.IsValid())
							AttackData.GetMutable<FHitboxAttackData>().SetSource(EExternalAttackDataSource::EADS_MontageHitbox);

						HitboxManager->ReceiveHitboxNotify(AnimDrivenSpecHandle, HitResult, TConstStructView<FHitboxAttackData>(AttackData));
					}
				}
			}
		}
	}
}
#if WITH_EDITOR
void ULQHitboxAnimNotifyState::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Preserve socket name when changing shape types
	static FName PreviousSocketName = NAME_None;

	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ULQHitboxAnimNotifyState, HitboxShape))
	{
		if (HitboxShape.IsValid())
		{
			// Store the current socket name before the change
			if (PreviousSocketName != NAME_None)
			{
				// Apply the previous socket name to the new shape
				HitboxShape.GetMutablePtr<FHitboxShapeBase>()->SocketName = PreviousSocketName;
			}
			// Update the stored socket name for next time
			PreviousSocketName = HitboxShape.Get<FHitboxShapeBase>().SocketName;
		}
		else
		{
			CurrentSockets.Empty();
			PreviousSocketName = NAME_None;
		}
	}
	else if (HitboxShape.IsValid())
	{
		// Update stored socket name when it changes directly
		PreviousSocketName = HitboxShape.Get<FHitboxShapeBase>().SocketName;
	}
}

FString ULQHitboxAnimNotifyState::GetNotifyName_Implementation() const
{
	return FString::Printf(TEXT("LQHitbox"));
}
#endif

TArray<FName> ULQHitboxAnimNotifyState::GetSocketNames()
{
	return CurrentSockets;
}
