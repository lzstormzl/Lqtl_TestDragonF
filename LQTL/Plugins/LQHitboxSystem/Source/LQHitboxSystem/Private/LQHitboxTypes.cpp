// Copyright Tale Weavers
#include "LQHitboxTypes.h"

#include "LQHitboxConsoleVariables.h"
#include "LQHitboxManager.h"
#include "Kismet/KismetSystemLibrary.h"

DEFINE_LOG_CATEGORY(LogLQHitboxSystem);

FHitboxHitResultWrapper::FHitboxHitResultWrapper(const FHitResult& HitResult)
{
	HitActorKey = TObjectKey<AActor>(HitResult.GetActor());
	HitComponentKey = TObjectKey<UPrimitiveComponent>(HitResult.GetComponent());
}

void FHitboxShapeBase::BeginTrace(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	MyMeshComp = MeshComp;
	if (!MyMeshComp->DoesSocketExist(SocketName))
	{
		TArray<USceneComponent*> OutChildrenComps;
		MyMeshComp->GetChildrenComponents(false, OutChildrenComps);
		for (USceneComponent* Comp : OutChildrenComps)
		{
			if (Comp && (Comp->IsA(UStaticMeshComponent::StaticClass()) || Comp->IsA(USkeletalMeshComponent::StaticClass())))
			{
				if (Comp->DoesSocketExist(SocketName))
				{
					MyMeshComp = Cast<UMeshComponent>(Comp);
					break;
				}
			}
		}
	}
}

bool FHitboxShape_Sphere::TickTrace(UObject* TraceWorldOwner, FHitResult& OutHitResult) const
{
	if (TraceWorldOwner && MyMeshComp)
	{
		FHitboxDebugSettings DebugSetting;
		ULQHitboxManager::GetCurrentDebugHitboxSettings(DebugSetting);

		auto SocketLocation = MyMeshComp->GetSocketLocation(SocketName);
		auto TraceResult = UKismetSystemLibrary::SphereTraceSingle(TraceWorldOwner->GetWorld(), SocketLocation, SocketLocation,
		                                                           Radius, TraceChannel, false, {Cast<AActor>(TraceWorldOwner)},
		                                                           DebugSetting.DrawDebugType, OutHitResult, true, DebugSetting.DebugTraceColor, DebugSetting.DebugTraceHitColor,
		                                                           DebugSetting.DebugDuration);

		return TraceResult;
	}
	return false;
}

bool FHitboxArray::TickTrace(UObject* TraceWorldOwner, FHitResult& OutHitResult) const

{
	for (const auto& HitboxShape : HitboxShapes)
	{
		if (HitboxShape.IsValid())
		{
			if (HitboxShape.Get<FHitboxShapeBase>().TickTrace(TraceWorldOwner, OutHitResult))
			{
				//Get the first hit possible
				return true;
			}
		}
	}
	return false;
}
