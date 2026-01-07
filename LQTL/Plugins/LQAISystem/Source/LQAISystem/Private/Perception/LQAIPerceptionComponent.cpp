// Copyright Tale Weavers

#include "Perception/LQAIPerceptionComponent.h"
#include "Perception/LQPerceptionTypes.h"
#include "DrawDebugHelpers.h"

ULQAIPerceptionComponent::ULQAIPerceptionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// ============================================================================
// Lifecycle
// ============================================================================

void ULQAIPerceptionComponent::BeginPlay()
{
	Super::BeginPlay();

	// Copy default handles to runtime and start them
	for (const TInstancedStruct<FLQAIPerceptionHandle>& DefaultHandle : DefaultHandles)
	{
		if (DefaultHandle.IsValid())
		{
			AddHandle(DefaultHandle);
		}
	}
}

void ULQAIPerceptionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Clean up all handles
	ClearHandles();

	Super::EndPlay(EndPlayReason);
}

void ULQAIPerceptionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Build cached context for this frame
	CachedContext = BuildContext();

	// Tick all enabled handles
	for (TInstancedStruct<FLQAIPerceptionHandle>& Handle : RuntimeHandles)
	{
		if (Handle.IsValid())
		{
			FLQAIPerceptionHandle* HandlePtr = Handle.GetMutablePtr();
			if (HandlePtr && HandlePtr->bEnabled)
			{
				HandlePtr->TickPerception(CachedContext, DeltaTime);
			}
		}
	}

	// Update current target based on weights
	UpdateCurrentTarget();

	// Debug visualization
	if (bDrawDebugPerception)
	{
		DrawDebugPerception(0.0f);
	}
}

// ============================================================================
// Handle Management
// ============================================================================

int32 ULQAIPerceptionComponent::AddHandle(const TInstancedStruct<FLQAIPerceptionHandle>& Handle)
{
	if (!Handle.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("ULQAIPerceptionComponent::AddHandle: Invalid handle for (%s)"), *GetOwner()->GetName());
		return INDEX_NONE;
	}

	const FLQAIPerceptionHandle* HandlePtr = Handle.GetPtr();
	if (!HandlePtr)
	{
		return INDEX_NONE;
	}

	// Check if handle with this tag already exists
	if (HandlePtr->HandleTag.IsValid() && HasHandle(HandlePtr->HandleTag))
	{
		UE_LOG(LogTemp, Warning, TEXT("ULQAIPerceptionComponent::AddHandle: Handle with tag (%s) already exists for (%s)"),
			*HandlePtr->HandleTag.ToString(), *GetOwner()->GetName());
		return INDEX_NONE;
	}

	// Add to runtime array
	const int32 NewIndex = RuntimeHandles.Add(Handle);

	// Call StartPerception
	const FLQAIPerceptionContext Context = BuildContext();
	if (FLQAIPerceptionHandle* MutableHandle = RuntimeHandles[NewIndex].GetMutablePtr())
	{
		MutableHandle->StartPerception(Context);
	}

	return NewIndex;
}

bool ULQAIPerceptionComponent::RemoveHandle(FGameplayTag HandleTag)
{
	if (!HandleTag.IsValid())
	{
		return false;
	}

	const FLQAIPerceptionContext Context = BuildContext();

	for (int32 i = 0; i < RuntimeHandles.Num(); ++i)
	{
		TInstancedStruct<FLQAIPerceptionHandle>& Handle = RuntimeHandles[i];
		if (Handle.IsValid())
		{
			const FLQAIPerceptionHandle* HandlePtr = Handle.GetPtr();
			if (HandlePtr && HandlePtr->HandleTag == HandleTag)
			{
				// Call EndPerception
				if (FLQAIPerceptionHandle* MutableHandle = Handle.GetMutablePtr())
				{
					MutableHandle->EndPerception(Context);
				}

				// Clear all perceptions for this handle
				ClearAllPerceptionsForHandle(HandleTag);

				// Remove from array
				RuntimeHandles.RemoveAt(i);
				return true;
			}
		}
	}

	return false;
}

void ULQAIPerceptionComponent::ClearHandles()
{
	const FLQAIPerceptionContext Context = BuildContext();

	// Call EndPerception on all handles
	for (TInstancedStruct<FLQAIPerceptionHandle>& Handle : RuntimeHandles)
	{
		if (Handle.IsValid())
		{
			if (FLQAIPerceptionHandle* HandlePtr = Handle.GetMutablePtr())
			{
				HandlePtr->EndPerception(Context);
			}
		}
	}

	RuntimeHandles.Empty();
	PerceivedActors.Empty();
	CurrentTarget.Reset();
	bTargetLocked = false;
}

bool ULQAIPerceptionComponent::HasHandle(FGameplayTag HandleTag) const
{
	if (!HandleTag.IsValid())
	{
		return false;
	}

	for (const TInstancedStruct<FLQAIPerceptionHandle>& Handle : RuntimeHandles)
	{
		if (Handle.IsValid())
		{
			const FLQAIPerceptionHandle* HandlePtr = Handle.GetPtr();
			if (HandlePtr && HandlePtr->HandleTag == HandleTag)
			{
				return true;
			}
		}
	}

	return false;
}

// ============================================================================
// Perception Reporting
// ============================================================================

void ULQAIPerceptionComponent::ReportPerception(FGameplayTag HandleTag, AActor* Target, float WeightModifier, bool bIsRetained)
{
	if (!Target || !HandleTag.IsValid())
	{
		return;
	}

	// Get base weight for this handle
	const float BaseWeight = GetHandleWeight(HandleTag);
	if (BaseWeight <= 0.0f)
	{
		return;
	}

	// Calculate effective weight
	const float EffectiveWeight = BaseWeight * FMath::Clamp(WeightModifier, 0.0f, 1.0f);

	// Get or create perception score for this target
	FLQAIPerceptionScore& Score = PerceivedActors.FindOrAdd(Target);

	// Track if this is a new perception for this handle
	const bool bWasPerceivingBefore = Score.PerceivingHandles.Contains(HandleTag);

	// Update handle's weight contribution
	Score.HandleWeights.Add(HandleTag, EffectiveWeight);

	// Update perceiving handles list
	if (!bWasPerceivingBefore)
	{
		Score.PerceivingHandles.AddUnique(HandleTag);
	}

	// Update retention flag
	if (bIsRetained)
	{
		Score.bAnyRetained = true;
	}

	// Recalculate total weight
	Score.RecalculateTotalWeight();

	// Fire perception gained delegate if this is first time
	if (!bWasPerceivingBefore)
	{
		OnPerceptionGained.Broadcast(Target, HandleTag, true);
	}
}

void ULQAIPerceptionComponent::ClearPerception(FGameplayTag HandleTag, AActor* Target)
{
	if (!Target || !HandleTag.IsValid())
	{
		return;
	}

	FLQAIPerceptionScore* Score = PerceivedActors.Find(Target);
	if (!Score)
	{
		return;
	}

	// Check if handle was perceiving
	const bool bWasPerceiving = Score->PerceivingHandles.Contains(HandleTag);
	if (!bWasPerceiving)
	{
		return;
	}

	// Remove handle's contribution
	Score->HandleWeights.Remove(HandleTag);
	Score->PerceivingHandles.Remove(HandleTag);

	// Recalculate retention flag
	Score->bAnyRetained = false;

	// Recalculate total weight
	Score->RecalculateTotalWeight();

	// Fire perception lost delegate
	OnPerceptionLost.Broadcast(Target, HandleTag, false);

	// Remove actor entirely if no handles perceive it
	if (Score->PerceivingHandles.Num() == 0)
	{
		PerceivedActors.Remove(Target);
	}
}

void ULQAIPerceptionComponent::ClearAllPerceptionsForHandle(FGameplayTag HandleTag)
{
	if (!HandleTag.IsValid())
	{
		return;
	}

	TArray<TWeakObjectPtr<AActor>> ActorsToClear;

	// Find all actors perceived by this handle
	for (const TPair<TWeakObjectPtr<AActor>, FLQAIPerceptionScore>& Pair : PerceivedActors)
	{
		if (Pair.Value.PerceivingHandles.Contains(HandleTag))
		{
			ActorsToClear.Add(Pair.Key);
		}
	}

	// Clear perception for each actor
	for (const TWeakObjectPtr<AActor>& Actor : ActorsToClear)
	{
		if (Actor.IsValid())
		{
			ClearPerception(HandleTag, Actor.Get());
		}
	}
}

// ============================================================================
// Queries
// ============================================================================

bool ULQAIPerceptionComponent::HasAnyPerception(AActor* Target) const
{
	if (!Target)
	{
		return false;
	}

	const FLQAIPerceptionScore* Score = PerceivedActors.Find(Target);
	return Score != nullptr && Score->PerceivingHandles.Num() > 0;
}

const FLQAIPerceptionScore* ULQAIPerceptionComponent::GetPerceptionScore(AActor* Target) const
{
	if (!Target)
	{
		return nullptr;
	}

	return PerceivedActors.Find(Target);
}

const TMap<TWeakObjectPtr<AActor>, FLQAIPerceptionScore>& ULQAIPerceptionComponent::GetPerceivedActors() const
{
	return PerceivedActors;
}

// ============================================================================
// Weight-Based Target Selection
// ============================================================================

AActor* ULQAIPerceptionComponent::GetHighestWeightTarget() const
{
	AActor* BestTarget = nullptr;
	float HighestWeight = 0.0f;

	for (const TPair<TWeakObjectPtr<AActor>, FLQAIPerceptionScore>& Pair : PerceivedActors)
	{
		if (Pair.Key.IsValid() && Pair.Value.TotalWeight > HighestWeight)
		{
			BestTarget = Pair.Key.Get();
			HighestWeight = Pair.Value.TotalWeight;
		}
	}

	return BestTarget;
}

float ULQAIPerceptionComponent::GetTargetWeight(AActor* Target) const
{
	if (!Target)
	{
		return 0.0f;
	}

	const FLQAIPerceptionScore* Score = PerceivedActors.Find(Target);
	return Score ? Score->TotalWeight : 0.0f;
}

AActor* ULQAIPerceptionComponent::GetCurrentTarget() const
{
	return CurrentTarget.Get();
}

// ============================================================================
// Target Lock
// ============================================================================

void ULQAIPerceptionComponent::SetTargetLockEnabled(bool bEnabled)
{
	bTargetLocked = bEnabled;

	if (!bTargetLocked)
	{
		// When unlocking, update to highest weight target
		UpdateCurrentTarget();
	}
}

bool ULQAIPerceptionComponent::IsTargetLocked() const
{
	return bTargetLocked;
}

void ULQAIPerceptionComponent::SetCurrentTarget(AActor* NewTarget)
{
	AActor* OldTarget = CurrentTarget.Get();

	if (OldTarget != NewTarget)
	{
		CurrentTarget = NewTarget;
		bTargetLocked = true;

		const float NewWeight = GetTargetWeight(NewTarget);
		OnCurrentTargetChanged.Broadcast(OldTarget, NewTarget, NewWeight);
	}
}

void ULQAIPerceptionComponent::ClearCurrentTarget()
{
	AActor* OldTarget = CurrentTarget.Get();

	if (OldTarget)
	{
		CurrentTarget.Reset();
		bTargetLocked = false;

		OnCurrentTargetChanged.Broadcast(OldTarget, nullptr, 0.0f);
	}
}

// ============================================================================
// Debug Visualization
// ============================================================================

void ULQAIPerceptionComponent::DrawDebugPerception(float Duration) const
{
	const AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	const UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const FVector OwnerLocation = Owner->GetActorLocation();
	const FVector DebugLocation = OwnerLocation + FVector(0.0f, 0.0f, 100.0f);

	// Build debug string for current target
	FString DebugText;

	AActor* Target = CurrentTarget.Get();
	if (Target)
	{
		const float Weight = GetTargetWeight(Target);

		DebugText += FString::Printf(TEXT("Current Target: %s\n"), *Target->GetName());
		DebugText += FString::Printf(TEXT("Total Weight: %.2f"), Weight);

		if (bTargetLocked)
		{
			DebugText += TEXT("\nLocked: Yes");
		}

		// Draw line to current target
		DrawDebugLine(World, OwnerLocation, Target->GetActorLocation(), FColor::Yellow, false, Duration, 0, 2.0f);
	}
	else
	{
		DebugText = TEXT("No Target");
	}

	// Draw debug string
	DrawDebugString(World, DebugLocation, DebugText, nullptr, FColor::White, Duration, true);
}

FLQAIPerceptionContext ULQAIPerceptionComponent::BuildContext() const
{
	FLQAIPerceptionContext Context;
	Context.PerceptionComponent = const_cast<ULQAIPerceptionComponent*>(this);
	return Context;
}

void ULQAIPerceptionComponent::UpdateCurrentTarget()
{
	AActor* OldTarget = CurrentTarget.Get();
	AActor* NewTarget = OldTarget;

	if (bTargetLocked)
	{
		// If locked, keep current target if still perceived
		if (OldTarget && !HasAnyPerception(OldTarget))
		{
			// Current target lost, find new highest weight
			NewTarget = GetHighestWeightTarget();
		}
	}
	else
	{
		// Not locked, always use highest weight target
		NewTarget = GetHighestWeightTarget();
	}

	// Update current target if changed
	if (NewTarget != OldTarget)
	{
		CurrentTarget = NewTarget;
		const float NewWeight = GetTargetWeight(NewTarget);
		OnCurrentTargetChanged.Broadcast(OldTarget, NewTarget, NewWeight);
	}
}

void ULQAIPerceptionComponent::CleanupStalePerceptions()
{
	TArray<TWeakObjectPtr<AActor>> StaleActors;

	// Find stale entries
	for (const TPair<TWeakObjectPtr<AActor>, FLQAIPerceptionScore>& Pair : PerceivedActors)
	{
		if (!Pair.Key.IsValid())
		{
			StaleActors.Add(Pair.Key);
		}
	}

	// Remove stale entries
	for (const TWeakObjectPtr<AActor>& StaleActor : StaleActors)
	{
		PerceivedActors.Remove(StaleActor);
	}

	// Clear current target if stale
	if (CurrentTarget.IsValid() == false)
	{
		CurrentTarget.Reset();
	}
}

float ULQAIPerceptionComponent::GetHandleWeight(FGameplayTag HandleTag) const
{
	if (!HandleTag.IsValid())
	{
		return 0.0f;
	}

	for (const TInstancedStruct<FLQAIPerceptionHandle>& Handle : RuntimeHandles)
	{
		if (Handle.IsValid())
		{
			const FLQAIPerceptionHandle* HandlePtr = Handle.GetPtr();
			if (HandlePtr && HandlePtr->HandleTag == HandleTag)
			{
				return HandlePtr->Weight;
			}
		}
	}

	return 0.0f;
}

