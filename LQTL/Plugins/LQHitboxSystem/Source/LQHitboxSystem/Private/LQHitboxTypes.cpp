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
		                                                           DebugSetting.DrawDebugType, OutHitResult, true, DebugSetting.DebugTraceColor,
		                                                           DebugSetting.DebugTraceHitColor,
		                                                           DebugSetting.DebugDuration);

		return TraceResult;
	}
	return false;
}

bool FHitboxShape_Box::TickTrace(UObject* TraceWorldOwner, FHitResult& OutHitResult) const
{
	if (TraceWorldOwner && MyMeshComp)
	{
		FHitboxDebugSettings DebugSetting;
		ULQHitboxManager::GetCurrentDebugHitboxSettings(DebugSetting);

		auto SocketTransform = MyMeshComp->GetSocketTransform(SocketName);
		auto SocketLocation = SocketTransform.GetLocation();

		// Convert local orientation to world space
		auto WorldRotation = (SocketTransform.GetRotation() * Orientation.Quaternion()).Rotator();

		auto TraceResult = UKismetSystemLibrary::BoxTraceSingle(TraceWorldOwner->GetWorld(), SocketLocation, SocketLocation,
		                                                        HalfExtent, WorldRotation, TraceChannel, false, {Cast<AActor>(TraceWorldOwner)},
		                                                        DebugSetting.DrawDebugType, OutHitResult, true, DebugSetting.DebugTraceColor,
		                                                        DebugSetting.DebugTraceHitColor,
		                                                        DebugSetting.DebugDuration);

		return TraceResult;
	}
	return false;
}

bool FHitboxShape_Capsule::TickTrace(UObject* TraceWorldOwner, FHitResult& OutHitResult) const
{
	if (TraceWorldOwner && MyMeshComp)
	{
		FHitboxDebugSettings DebugSetting;
		ULQHitboxManager::GetCurrentDebugHitboxSettings(DebugSetting);

		auto SocketTransform = MyMeshComp->GetSocketTransform(SocketName);
		auto SocketLocation = SocketTransform.GetLocation();

		// Calculate the capsule axis direction in local space, then transform to world space
		// Default capsule is vertical (Z-axis), rotated by orientation in local space
		auto LocalAxisDirection = Orientation.RotateVector(FVector(0.f, 0.f, 1.f));
		auto WorldAxisDirection = SocketTransform.TransformVectorNoScale(LocalAxisDirection);

		// Calculate start/end points: center ± axis * half height
		auto Start = SocketLocation + WorldAxisDirection * HalfHeight;
		auto End = SocketLocation - WorldAxisDirection * HalfHeight;

		auto TraceResult = UKismetSystemLibrary::SphereTraceSingle(TraceWorldOwner->GetWorld(), Start, End,
		                                                           Radius, TraceChannel, false, {Cast<AActor>(TraceWorldOwner)},
		                                                           DebugSetting.DrawDebugType, OutHitResult, true, DebugSetting.DebugTraceColor,
		                                                           DebugSetting.DebugTraceHitColor,
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

FHitboxAttackData::FHitboxAttackData(EExternalAttackDataSource Source)
{
	SetSource(Source);
}

void FHitboxAttackData::SetSource(EExternalAttackDataSource Source)
{
	if (!PreApplyDamageData.IsValid())
	{
		PreApplyDamageData = TInstancedStruct<FPreApplyDamageData>().Make();
	}
	PreApplyDamageData.GetMutablePtr<FPreApplyDamageData>()->MyDataSource = Source;

	if (!PostApplyDamageData.IsValid())
	{
		PostApplyDamageData = TInstancedStruct<FPostApplyDamageData>().Make();
	}
	PostApplyDamageData.GetMutablePtr<FPostApplyDamageData>()->MyDataSource = Source;
}
