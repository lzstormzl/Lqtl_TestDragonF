// Copyright Tale Weavers

#include "Perception/LQAIPerceptionHandle.h"
#include "Perception/LQAIPerceptionComponent.h"
#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "Engine/OverlapResult.h"
#include "GameplayTagAssetInterface.h"
#include "GameFramework/Pawn.h"

namespace LQAI::Perception
{
	UE_DEFINE_GAMEPLAY_TAG(Range, "LQ.AI.Perception.Handle.Range");
	UE_DEFINE_GAMEPLAY_TAG(Sight, "LQ.AI.Perception.Handle.Sight");
	UE_DEFINE_GAMEPLAY_TAG(Tag, "LQ.AI.Perception.Handle.Tag");
}

void FLQAIPerceptionHandle::StartPerception(const FLQAIPerceptionContext& Context)
{
	// Base implementation does nothing - override in derived classes
}

void FLQAIPerceptionHandle::TickPerception(const FLQAIPerceptionContext& Context, float DeltaTime)
{
	// Base implementation does nothing - override in derived classes
}

void FLQAIPerceptionHandle::EndPerception(const FLQAIPerceptionContext& Context)
{
	// Base implementation does nothing - override in derived classes
}

float FLQAIPerceptionHandle::GetWeightModifier(AActor* Target, const FLQAIPerceptionContext& Context) const
{
	// Base implementation returns full weight
	return 1.0f;
}

float FLQAIPerceptionHandle::ComputeEffectiveWeight(AActor* Target, const FLQAIPerceptionContext& Context) const
{
	return Weight * GetWeightModifier(Target, Context);
}

void FLQAIPerceptionHandle::Reset()
{
	// Base implementation does nothing - override in derived classes
}

FString FLQAIPerceptionHandle::GetDebugName() const
{
	return TEXT("FLQAIPerceptionHandle");
}

void FLQAIPerceptionHandle::DrawDebug(const FLQAIPerceptionContext& Context, float Duration) const
{
	// Base implementation does nothing - override in derived classes
}

FLQAIPerceptionHandle_Range::FLQAIPerceptionHandle_Range()
{
	HandleTag = LQAI::Perception::Range;
}

void FLQAIPerceptionHandle_Range::TickPerception(const FLQAIPerceptionContext& Context, float DeltaTime)
{
	if (!bEnabled || !Context.IsValid())
	{
		return;
	}

	ULQAIPerceptionComponent* PerceptionComp = Context.PerceptionComponent.Get();
	if (!PerceptionComp)
	{
		return;
	}

	AActor* Owner = Context.GetOwner();
	UWorld* World = Context.GetWorld();
	if (!Owner || !World)
	{
		return;
	}

	const FVector OwnerLocation = Owner->GetActorLocation();
	const float CurrentTime = World->GetTimeSeconds();

	// Track which targets we currently perceive this frame
	TSet<TWeakObjectPtr<AActor>> NewlyPerceivedTargets;

	// Scan for targets
	if (TargetClasses.Num() > 0)
	{
		// Scan specific classes
		for (TSubclassOf<AActor> TargetClass : TargetClasses)
		{
			if (!TargetClass)
			{
				continue;
			}

			for (TActorIterator<AActor> It(World, TargetClass); It; ++It)
			{
				AActor* Target = *It;
				if (Target && Target != Owner)
				{
					const bool bIsValid = CheckTarget(Target, Context);

					if (bIsValid)
					{
						const float WeightMod = GetWeightModifier(Target, Context);
						PerceptionComp->ReportPerception(HandleTag, Target, WeightMod, false);
						NewlyPerceivedTargets.Add(Target);
						TargetLastPerceptionTime.Add(Target, CurrentTime);
					}

					// Draw debug visualization
					if (bDrawDebugPerception)
					{
						const FVector TargetLocation = Target->GetActorLocation();
						const float Distance = FVector::Dist(OwnerLocation, TargetLocation);
						const FVector MidPoint = (OwnerLocation + TargetLocation) * 0.5f;

						if (bIsValid)
						{
							// Green line for valid target
							DrawDebugLine(World, OwnerLocation, TargetLocation, FColor::Green, false, -1.0f, 0, 2.0f);
							// Draw distance at mid point
							DrawDebugString(World, MidPoint, FString::Printf(TEXT("%.0f"), Distance), nullptr, FColor::Green, 0.0f, true);
						}
						else
						{
							// Red line for invalid target
							DrawDebugLine(World, OwnerLocation, TargetLocation, FColor::Red, false, -1.0f, 0, 1.0f);

							// Determine reason why not valid
							FString Reason;
							if (Distance < MinDistance)
							{
								Reason = TEXT("Too Close");
							}
							else if (Distance > DetectionRadius)
							{
								Reason = TEXT("Too Far");
							}
							else if (bRequireLineOfSight)
							{
								// Check if it's LOS blocking
								FHitResult HitResult;
								FCollisionQueryParams TraceQueryParams;
								TraceQueryParams.AddIgnoredActor(Owner);
								TraceQueryParams.AddIgnoredActor(Target);

								const bool bHit = World->LineTraceSingleByChannel(HitResult, OwnerLocation, TargetLocation, TraceChannel, TraceQueryParams);
								if (bHit)
								{
									Reason = TEXT("No LOS");
								}
							}

							// Draw reason at mid point
							DrawDebugString(World, MidPoint, Reason, nullptr, FColor::Red, 0.0f, true);
						}
					}
				}
			}
		}
	}
	else
	{
		// Scan all actors using sphere overlap
		TArray<FOverlapResult> OverlapResults;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(Owner);

		const bool bOverlapped = World->OverlapMultiByChannel(OverlapResults, OwnerLocation, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(DetectionRadius), QueryParams);

		if (bOverlapped)
		{
			for (const FOverlapResult& Result : OverlapResults)
			{
				AActor* Target = Result.GetActor();
				if (Target && Target != Owner)
				{
					const bool bIsValid = CheckTarget(Target, Context);

					if (bIsValid)
					{
						const float WeightMod = GetWeightModifier(Target, Context);
						PerceptionComp->ReportPerception(HandleTag, Target, WeightMod, false);
						NewlyPerceivedTargets.Add(Target);
						TargetLastPerceptionTime.Add(Target, CurrentTime);
					}

					// Draw debug visualization
					if (bDrawDebugPerception)
					{
						const FVector TargetLocation = Target->GetActorLocation();
						const float Distance = FVector::Dist(OwnerLocation, TargetLocation);
						const FVector MidPoint = (OwnerLocation + TargetLocation) * 0.5f;

						if (bIsValid)
						{
							// Green line for valid target
							DrawDebugLine(World, OwnerLocation, TargetLocation, FColor::Green, false, -1.0f, 0, 2.0f);
							// Draw distance at mid point
							DrawDebugString(World, MidPoint, FString::Printf(TEXT("%.0f"), Distance), nullptr, FColor::Green, 0.0f, true);
						}
						else
						{
							// Red line for invalid target
							DrawDebugLine(World, OwnerLocation, TargetLocation, FColor::Red, false, -1.0f, 0, 1.0f);

							// Determine reason why not valid
							FString Reason;
							if (Distance < MinDistance)
							{
								Reason = TEXT("Too Close");
							}
							else if (Distance > DetectionRadius)
							{
								Reason = TEXT("Too Far");
							}
							else if (bRequireLineOfSight)
							{
								// Check if it's LOS blocking
								FHitResult HitResult;
								FCollisionQueryParams TraceQueryParams;
								TraceQueryParams.AddIgnoredActor(Owner);
								TraceQueryParams.AddIgnoredActor(Target);

								const bool bHit = World->LineTraceSingleByChannel(HitResult, OwnerLocation, TargetLocation, TraceChannel, TraceQueryParams);
								if (bHit)
								{
									Reason = TEXT("No LOS");
								}
							}

							// Draw reason at mid point
							DrawDebugString(World, MidPoint, Reason, nullptr, FColor::Red, 0.0f, true);
						}
					}
				}
			}
		}
	}

	// Clear perception for targets we no longer see (memory-based forgetting)
	TArray<TWeakObjectPtr<AActor>> TargetsToForget;
	for (const TWeakObjectPtr<AActor>& PreviousTarget : CurrentlyPerceivedTargets)
	{
		if (!PreviousTarget.IsValid())
		{
			TargetsToForget.Add(PreviousTarget);
			continue;
		}

		// If target is still being perceived, skip
		if (NewlyPerceivedTargets.Contains(PreviousTarget))
		{
			continue;
		}

		// Check if memory duration has expired
		const float* LastPerceptionTime = TargetLastPerceptionTime.Find(PreviousTarget);
		if (LastPerceptionTime)
		{
			const float TimeSinceLastPerception = CurrentTime - *LastPerceptionTime;
			if (TimeSinceLastPerception >= PerceptionMemoryDuration)
			{
				// Memory expired, forget this target
				PerceptionComp->ClearPerception(HandleTag, PreviousTarget.Get());
				TargetsToForget.Add(PreviousTarget);
			}
		}
		else
		{
			// No memory record, clear immediately
			PerceptionComp->ClearPerception(HandleTag, PreviousTarget.Get());
			TargetsToForget.Add(PreviousTarget);
		}
	}

	// Remove forgotten targets from tracking
	for (const TWeakObjectPtr<AActor>& ForgottenTarget : TargetsToForget)
	{
		CurrentlyPerceivedTargets.Remove(ForgottenTarget);
		TargetLastPerceptionTime.Remove(ForgottenTarget);
	}

	// Add newly perceived targets to our tracked set
	CurrentlyPerceivedTargets.Append(NewlyPerceivedTargets);
}

float FLQAIPerceptionHandle_Range::GetWeightModifier(AActor* Target, const FLQAIPerceptionContext& Context) const
{
	if (!Target || !Context.IsValid())
	{
		return 0.0f;
	}

	AActor* Owner = Context.GetOwner();
	if (!Owner)
	{
		return 0.0f;
	}

	const float Distance = FVector::Dist(Owner->GetActorLocation(), Target->GetActorLocation());

	// If outside detection radius, no perception
	if (Distance > DetectionRadius)
	{
		return 0.0f;
	}

	// If inside min distance, full weight
	if (Distance <= MinDistance)
	{
		return 1.0f;
	}

	// Linear falloff from MinDistance to DetectionRadius
	const float Range = DetectionRadius - MinDistance;
	if (Range <= 0.0f)
	{
		return 1.0f;
	}

	const float NormalizedDistance = (Distance - MinDistance) / Range;
	return FMath::Clamp(1.0f - NormalizedDistance, 0.0f, 1.0f);
}

void FLQAIPerceptionHandle_Range::Reset()
{
	CurrentlyPerceivedTargets.Empty();
	TargetLastPerceptionTime.Empty();
}

FString FLQAIPerceptionHandle_Range::GetDebugName() const
{
	return FString::Printf(TEXT("Range(%.0f)"), DetectionRadius);
}

void FLQAIPerceptionHandle_Range::DrawDebug(const FLQAIPerceptionContext& Context, float Duration) const
{
	// Each handle implements its own debug visualization
}

bool FLQAIPerceptionHandle_Range::CheckTarget(AActor* Target, const FLQAIPerceptionContext& Context) const
{
	if (!Target || !Context.IsValid())
	{
		return false;
	}

	AActor* Owner = Context.GetOwner();
	UWorld* World = Context.GetWorld();
	if (!Owner || !World)
	{
		return false;
	}

	const FVector OwnerLocation = Owner->GetActorLocation();
	const FVector TargetLocation = Target->GetActorLocation();
	const float Distance = FVector::Dist(OwnerLocation, TargetLocation);

	// Check distance range
	if (Distance < MinDistance || Distance > DetectionRadius)
	{
		return false;
	}

	// Check line of sight if required
	if (bRequireLineOfSight)
	{
		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(Owner);
		QueryParams.AddIgnoredActor(Target);

		const bool bHit = World->LineTraceSingleByChannel(HitResult, OwnerLocation, TargetLocation, TraceChannel, QueryParams);

		// If trace hit something, we don't have line of sight
		if (bHit)
		{
			return false;
		}
	}

	return true;
}

// ============================================================================
// FLQAIPerceptionHandle_Sight
// ============================================================================

FLQAIPerceptionHandle_Sight::FLQAIPerceptionHandle_Sight()
{
	HandleTag = LQAI::Perception::Sight;
}

void FLQAIPerceptionHandle_Sight::TickPerception(const FLQAIPerceptionContext& Context, float DeltaTime)
{
	if (!bEnabled || !Context.IsValid())
	{
		return;
	}

	ULQAIPerceptionComponent* PerceptionComp = Context.PerceptionComponent.Get();
	if (!PerceptionComp)
	{
		return;
	}

	AActor* Owner = Context.GetOwner();
	UWorld* World = Context.GetWorld();
	if (!Owner || !World)
	{
		return;
	}

	const FVector OwnerLocation = bUseEyeLocation ? GetOwnerEyeLocation(Owner) : Owner->GetActorLocation();
	const FVector OwnerForward = Owner->GetActorForwardVector();
	const float CurrentTime = World->GetTimeSeconds();

	// Track which targets we currently perceive this frame
	TSet<TWeakObjectPtr<AActor>> NewlyPerceivedTargets;

	// Scan for targets
	if (TargetClasses.Num() > 0)
	{
		// Scan specific classes
		for (TSubclassOf<AActor> TargetClass : TargetClasses)
		{
			if (!TargetClass)
			{
				continue;
			}

			for (TActorIterator<AActor> It(World, TargetClass); It; ++It)
			{
				AActor* Target = *It;
				if (Target && Target != Owner && CheckTarget(Target, Context, OwnerLocation, OwnerForward))
				{
					const float WeightMod = GetWeightModifier(Target, Context);
					PerceptionComp->ReportPerception(HandleTag, Target, WeightMod, false);
					NewlyPerceivedTargets.Add(Target);
					TargetLastPerceptionTime.Add(Target, CurrentTime);
				}
			}
		}
	}
	else
	{
		// Scan all actors using sphere overlap
		TArray<FOverlapResult> OverlapResults;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(Owner);

		const bool bOverlapped = World->OverlapMultiByChannel(OverlapResults, OwnerLocation, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(SightRadius), QueryParams);

		if (bOverlapped)
		{
			for (const FOverlapResult& Result : OverlapResults)
			{
				AActor* Target = Result.GetActor();
				if (Target && Target != Owner && CheckTarget(Target, Context, OwnerLocation, OwnerForward))
				{
					const float WeightMod = GetWeightModifier(Target, Context);
					PerceptionComp->ReportPerception(HandleTag, Target, WeightMod, false);
					NewlyPerceivedTargets.Add(Target);
					TargetLastPerceptionTime.Add(Target, CurrentTime);
				}
			}
		}
	}

	// Clear perception for targets we no longer see (memory-based forgetting)
	TArray<TWeakObjectPtr<AActor>> TargetsToForget;
	for (const TWeakObjectPtr<AActor>& PreviousTarget : CurrentlyPerceivedTargets)
	{
		if (!PreviousTarget.IsValid())
		{
			TargetsToForget.Add(PreviousTarget);
			continue;
		}

		// If target is still being perceived, skip
		if (NewlyPerceivedTargets.Contains(PreviousTarget))
		{
			continue;
		}

		// Check if memory duration has expired
		const float* LastPerceptionTime = TargetLastPerceptionTime.Find(PreviousTarget);
		if (LastPerceptionTime)
		{
			const float TimeSinceLastPerception = CurrentTime - *LastPerceptionTime;
			if (TimeSinceLastPerception >= PerceptionMemoryDuration)
			{
				// Memory expired, forget this target
				PerceptionComp->ClearPerception(HandleTag, PreviousTarget.Get());
				TargetsToForget.Add(PreviousTarget);
			}
		}
		else
		{
			// No memory record, clear immediately
			PerceptionComp->ClearPerception(HandleTag, PreviousTarget.Get());
			TargetsToForget.Add(PreviousTarget);
		}
	}

	// Remove forgotten targets from tracking
	for (const TWeakObjectPtr<AActor>& ForgottenTarget : TargetsToForget)
	{
		CurrentlyPerceivedTargets.Remove(ForgottenTarget);
		TargetLastPerceptionTime.Remove(ForgottenTarget);
	}

	// Add newly perceived targets to our tracked set
	CurrentlyPerceivedTargets.Append(NewlyPerceivedTargets);
}

float FLQAIPerceptionHandle_Sight::GetWeightModifier(AActor* Target, const FLQAIPerceptionContext& Context) const
{
	if (!Target || !Context.IsValid())
	{
		return 0.0f;
	}

	AActor* Owner = Context.GetOwner();
	if (!Owner)
	{
		return 0.0f;
	}

	const FVector OwnerLocation = bUseEyeLocation ? GetOwnerEyeLocation(Owner) : Owner->GetActorLocation();
	const FVector OwnerForward = Owner->GetActorForwardVector();
	const FVector TargetLocation = Target->GetActorLocation();

	const float Distance = FVector::Dist(OwnerLocation, TargetLocation);

	// If outside sight radius, no perception
	if (Distance > SightRadius)
	{
		return 0.0f;
	}

	// Distance factor: closer = higher weight (1.0 at 0, 0.0 at SightRadius)
	const float DistanceFactor = FMath::Clamp(1.0f - (Distance / SightRadius), 0.0f, 1.0f);

	// Angle factor: centered = higher weight
	const FVector DirectionToTarget = (TargetLocation - OwnerLocation).GetSafeNormal();
	const float DotProduct = FVector::DotProduct(OwnerForward, DirectionToTarget);

	// Convert half-angle to cosine for comparison
	const float CosHalfAngle = FMath::Cos(FMath::DegreesToRadians(SightHalfAngle));

	// If outside cone, no perception
	if (DotProduct < CosHalfAngle)
	{
		return 0.0f;
	}

	// Angle factor: map dot product from [CosHalfAngle, 1.0] to [0.0, 1.0]
	// Center of cone (dot=1.0) gets 1.0, edge of cone (dot=CosHalfAngle) gets 0.0
	const float AngleFactor = FMath::Clamp((DotProduct - CosHalfAngle) / (1.0f - CosHalfAngle), 0.0f, 1.0f);

	// Combine factors: both distance and angle matter
	return DistanceFactor * AngleFactor;
}

void FLQAIPerceptionHandle_Sight::Reset()
{
	CurrentlyPerceivedTargets.Empty();
	TargetLastPerceptionTime.Empty();
}

FString FLQAIPerceptionHandle_Sight::GetDebugName() const
{
	return FString::Printf(TEXT("Sight(%.0f/%.0f°)"), SightRadius, SightHalfAngle);
}

void FLQAIPerceptionHandle_Sight::DrawDebug(const FLQAIPerceptionContext& Context, float Duration) const
{
	// Each handle implements its own debug visualization
}

bool FLQAIPerceptionHandle_Sight::CheckTarget(AActor* Target, const FLQAIPerceptionContext& Context, const FVector& OwnerLocation, const FVector& OwnerForward) const
{
	if (!Target || !Context.IsValid())
	{
		return false;
	}

	AActor* Owner = Context.GetOwner();
	UWorld* World = Context.GetWorld();
	if (!Owner || !World)
	{
		return false;
	}

	const FVector TargetLocation = Target->GetActorLocation();
	const float Distance = FVector::Dist(OwnerLocation, TargetLocation);

	// Check distance
	if (Distance > SightRadius)
	{
		return false;
	}

	// Check if in cone
	const FVector DirectionToTarget = (TargetLocation - OwnerLocation).GetSafeNormal();
	const float DotProduct = FVector::DotProduct(OwnerForward, DirectionToTarget);
	const float CosHalfAngle = FMath::Cos(FMath::DegreesToRadians(SightHalfAngle));

	if (DotProduct < CosHalfAngle)
	{
		return false;
	}

	// Check line of sight
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Owner);
	QueryParams.AddIgnoredActor(Target);

	const bool bHit = World->LineTraceSingleByChannel(HitResult, OwnerLocation, TargetLocation, TraceChannel, QueryParams);

	// If trace hit something, we don't have line of sight
	if (bHit)
	{
		return false;
	}

	return true;
}

FVector FLQAIPerceptionHandle_Sight::GetOwnerEyeLocation(AActor* Owner) const
{
	if (!Owner)
	{
		return FVector::ZeroVector;
	}

	// Try to get eye location from pawn
	if (APawn* Pawn = Cast<APawn>(Owner))
	{
		return Pawn->GetPawnViewLocation();
	}

	// Fallback to actor location with offset
	return Owner->GetActorLocation() + FVector(0.0f, 0.0f, 80.0f);
}

// ============================================================================
// FLQAIPerceptionHandle_Tag
// ============================================================================

FLQAIPerceptionHandle_Tag::FLQAIPerceptionHandle_Tag()
{
	HandleTag = LQAI::Perception::Tag;
}

void FLQAIPerceptionHandle_Tag::StartPerception(const FLQAIPerceptionContext& Context)
{
	// Event-driven perception could bind to tag change events here
	// For now, we'll use tick-based scanning as a simpler implementation
}

void FLQAIPerceptionHandle_Tag::TickPerception(const FLQAIPerceptionContext& Context, float DeltaTime)
{
	if (!bEnabled || !Context.IsValid())
	{
		return;
	}

	ULQAIPerceptionComponent* PerceptionComp = Context.PerceptionComponent.Get();
	if (!PerceptionComp)
	{
		return;
	}

	AActor* Owner = Context.GetOwner();
	UWorld* World = Context.GetWorld();
	if (!Owner || !World)
	{
		return;
	}

	const float CurrentTime = World->GetTimeSeconds();

	// Track which targets we currently perceive this frame
	TSet<TWeakObjectPtr<AActor>> NewlyPerceivedTargets;

	// Scan for targets
	if (TargetClasses.Num() > 0)
	{
		// Scan specific classes
		for (TSubclassOf<AActor> TargetClass : TargetClasses)
		{
			if (!TargetClass)
			{
				continue;
			}

			for (TActorIterator<AActor> It(World, TargetClass); It; ++It)
			{
				AActor* Target = *It;
				if (Target && Target != Owner && CheckTarget(Target, Context))
				{
					const float WeightMod = GetWeightModifier(Target, Context);
					PerceptionComp->ReportPerception(HandleTag, Target, WeightMod, false);
					NewlyPerceivedTargets.Add(Target);
					TargetLastPerceptionTime.Add(Target, CurrentTime);
				}
			}
		}
	}
	else
	{
		// Scan all actors in world
		for (TActorIterator<AActor> It(World); It; ++It)
		{
			AActor* Target = *It;
			if (Target && Target != Owner && CheckTarget(Target, Context))
			{
				const float WeightMod = GetWeightModifier(Target, Context);
				PerceptionComp->ReportPerception(HandleTag, Target, WeightMod, false);
				NewlyPerceivedTargets.Add(Target);
				TargetLastPerceptionTime.Add(Target, CurrentTime);
			}
		}
	}

	// Clear perception for targets we no longer perceive (memory-based forgetting)
	TArray<TWeakObjectPtr<AActor>> TargetsToForget;
	for (const TWeakObjectPtr<AActor>& PreviousTarget : CurrentlyPerceivedTargets)
	{
		if (!PreviousTarget.IsValid())
		{
			TargetsToForget.Add(PreviousTarget);
			continue;
		}

		// If target is still being perceived, skip
		if (NewlyPerceivedTargets.Contains(PreviousTarget))
		{
			continue;
		}

		// Check if memory duration has expired
		const float* LastPerceptionTime = TargetLastPerceptionTime.Find(PreviousTarget);
		if (LastPerceptionTime)
		{
			const float TimeSinceLastPerception = CurrentTime - *LastPerceptionTime;
			if (TimeSinceLastPerception >= PerceptionMemoryDuration)
			{
				// Memory expired, forget this target
				PerceptionComp->ClearPerception(HandleTag, PreviousTarget.Get());
				TargetsToForget.Add(PreviousTarget);
			}
		}
		else
		{
			// No memory record, clear immediately
			PerceptionComp->ClearPerception(HandleTag, PreviousTarget.Get());
			TargetsToForget.Add(PreviousTarget);
		}
	}

	// Remove forgotten targets from tracking
	for (const TWeakObjectPtr<AActor>& ForgottenTarget : TargetsToForget)
	{
		CurrentlyPerceivedTargets.Remove(ForgottenTarget);
		TargetLastPerceptionTime.Remove(ForgottenTarget);
	}

	// Add newly perceived targets to our tracked set
	CurrentlyPerceivedTargets.Append(NewlyPerceivedTargets);
}

void FLQAIPerceptionHandle_Tag::EndPerception(const FLQAIPerceptionContext& Context)
{
	// Cleanup - could unbind events here if we were using event-driven approach
	CurrentlyPerceivedTargets.Empty();
	TargetLastPerceptionTime.Empty();
}

float FLQAIPerceptionHandle_Tag::GetWeightModifier(AActor* Target, const FLQAIPerceptionContext& Context) const
{
	if (!Target || !Context.IsValid())
	{
		return 0.0f;
	}

	// Tag perception is binary - either has tags or doesn't
	if (!CheckTargetTags(Target))
	{
		return 0.0f;
	}

	// If range requirement is enabled, scale by distance
	if (bRequireRange)
	{
		AActor* Owner = Context.GetOwner();
		if (!Owner)
		{
			return 0.0f;
		}

		const float Distance = FVector::Dist(Owner->GetActorLocation(), Target->GetActorLocation());

		// If outside range, no perception
		if (Distance > MaxRange)
		{
			return 0.0f;
		}

		// Scale by distance: closer = higher weight
		return FMath::Clamp(1.0f - (Distance / MaxRange), 0.0f, 1.0f);
	}

	// No range requirement, full weight
	return 1.0f;
}

void FLQAIPerceptionHandle_Tag::Reset()
{
	CurrentlyPerceivedTargets.Empty();
	TargetLastPerceptionTime.Empty();
}

FString FLQAIPerceptionHandle_Tag::GetDebugName() const
{
	return FString::Printf(TEXT("Tag(%s)"), *RequiredTagQuery.GetDescription());
}

void FLQAIPerceptionHandle_Tag::DrawDebug(const FLQAIPerceptionContext& Context, float Duration) const
{
	// Each handle implements its own debug visualization
}

bool FLQAIPerceptionHandle_Tag::CheckTargetTags(AActor* Target) const
{
	if (!Target)
	{
		return false;
	}

	// Try to get gameplay tag container from actor
	if (IGameplayTagAssetInterface* TagInterface = Cast<IGameplayTagAssetInterface>(Target))
	{
		FGameplayTagContainer ActorTags;
		TagInterface->GetOwnedGameplayTags(ActorTags);
		return RequiredTagQuery.Matches(ActorTags);
	}

	return false;
}

bool FLQAIPerceptionHandle_Tag::CheckTarget(AActor* Target, const FLQAIPerceptionContext& Context) const
{
	if (!Target || !Context.IsValid())
	{
		return false;
	}

	// Check tags first (fast rejection)
	if (!CheckTargetTags(Target))
	{
		return false;
	}

	// Check range if required
	if (bRequireRange)
	{
		AActor* Owner = Context.GetOwner();
		if (!Owner)
		{
			return false;
		}

		const float Distance = FVector::Dist(Owner->GetActorLocation(), Target->GetActorLocation());
		if (Distance > MaxRange)
		{
			return false;
		}
	}

	return true;
}

