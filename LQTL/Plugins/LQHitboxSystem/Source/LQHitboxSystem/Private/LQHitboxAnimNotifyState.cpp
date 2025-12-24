// Copyright Tale Weavers


#include "LQHitboxAnimNotifyState.h"

#include "LQHitboxManager.h"

void ULQHitboxAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
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
#endif

		HitboxManager = MeshComp->GetOwner()->FindComponentByClass<ULQHitboxManager>();
		if (HitboxShape.IsValid())
		{
			HitboxShape.GetMutablePtr<FHitboxShapeBase>()->BeginTrace(MeshComp, Animation, TotalDuration);
		}
	}
}

void ULQHitboxAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	AlreadyHitActors.Empty();
}

void ULQHitboxAnimNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
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
						HitboxManager->ReceiveHitboxNotify(HitResult);
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
	if (!HitboxShape.IsValid())
		CurrentSockets.Empty();
}
#endif

TArray<FName> ULQHitboxAnimNotifyState::GetSocketNames()
{
	return CurrentSockets;
}
