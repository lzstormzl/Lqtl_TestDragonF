// Copyright Tale Weavers

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InstancedStruct.h"
#include "Perception/LQAIPerceptionHandle.h"
#include "LQAIPerceptionComponent.generated.h"

// Delegate for perception state changes
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnLQAIPerceptionChanged, AActor*, TargetActor, FGameplayTag, HandleTag, bool, bPerceived);

// Delegate for current target changes
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnLQAICurrentTargetChanged, AActor*, OldTarget, AActor*, NewTarget, float, NewTargetWeight);

/**
 * Component that manages perception handles for an AI actor
 * Handles detect targets and report perception, component aggregates weights for target selection
 */
UCLASS(ClassGroup = (LQAI), meta = (BlueprintSpawnableComponent))
class LQAISYSTEM_API ULQAIPerceptionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULQAIPerceptionComponent();

	// === Handle Management ===

	/**
	 * Adds a perception handle, calls StartPerception, returns index
	 * @param	Handle		The perception handle to add
	 * @return	Index of the added handle in RuntimeHandles array
	 */
	UFUNCTION(BlueprintCallable, Category = "LQAI|Perception")
	int32 AddHandle(const TInstancedStruct<FLQAIPerceptionHandle>& Handle);

	/**
	 * Removes handle by tag, calls EndPerception, returns true if found
	 * @param	HandleTag	Tag identifying the handle to remove
	 * @return	true if handle was found and removed
	 */
	UFUNCTION(BlueprintCallable, Category = "LQAI|Perception")
	bool RemoveHandle(FGameplayTag HandleTag);

	// Removes all handles
	UFUNCTION(BlueprintCallable, Category = "LQAI|Perception")
	void ClearHandles();

	/**
	 * Returns true if handle with tag exists
	 * @param	HandleTag	Tag identifying the handle
	 * @return	true if handle exists
	 */
	UFUNCTION(BlueprintPure, Category = "LQAI|Perception")
	bool HasHandle(FGameplayTag HandleTag) const;

	// === Perception Reporting (Called by Handles) ===

	/**
	 * Report that a handle perceives a target with given weight modifier
	 * Adds/updates the target in PerceivedActors map
	 * @param	HandleTag		Tag identifying the reporting handle
	 * @param	Target			The actor being perceived
	 * @param	WeightModifier	Weight modifier (0.0-1.0) from handle
	 * @param	bIsRetained		Whether this perception is retained (vs direct)
	 */
	UFUNCTION(BlueprintCallable, Category = "LQAI|Perception")
	void ReportPerception(FGameplayTag HandleTag, AActor* Target, float WeightModifier, bool bIsRetained = false);

	/**
	 * Report that a handle no longer perceives a target
	 * Removes handle's contribution from the target's score
	 * @param	HandleTag	Tag identifying the reporting handle
	 * @param	Target		The actor no longer perceived
	 */
	UFUNCTION(BlueprintCallable, Category = "LQAI|Perception")
	void ClearPerception(FGameplayTag HandleTag, AActor* Target);

	/**
	 * Clear all perceptions reported by a specific handle
	 * @param	HandleTag	Tag identifying the handle
	 */
	UFUNCTION(BlueprintCallable, Category = "LQAI|Perception")
	void ClearAllPerceptionsForHandle(FGameplayTag HandleTag);

	// === Queries ===

	/**
	 * Returns true if any handle perceives the target
	 * @param	Target	The actor to check
	 * @return	true if target is perceived by any handle
	 */
	UFUNCTION(BlueprintPure, Category = "LQAI|Perception")
	bool HasAnyPerception(AActor* Target) const;

	/**
	 * Gets the perception score for a target (or nullptr if not perceived)
	 * @param	Target	The actor to get score for
	 * @return	Pointer to score struct, or nullptr if not perceived
	 */
	const FLQAIPerceptionScore* GetPerceptionScore(AActor* Target) const;

	/**
	 * Gets all perceived actors (map reference)
	 * @return	Map of perceived actors to their scores
	 */
	const TMap<TWeakObjectPtr<AActor>, FLQAIPerceptionScore>& GetPerceivedActors() const;

	// === Weight-Based Target Selection ===

	/**
	 * Returns the actor with highest total weight (or nullptr if none perceived)
	 * @return	Highest weight actor, or nullptr
	 */
	UFUNCTION(BlueprintCallable, Category = "LQAI|Perception|Targeting")
	AActor* GetHighestWeightTarget() const;

	/**
	 * Gets the total weight for a specific target (0 if not perceived)
	 * @param	Target	The actor to get weight for
	 * @return	Total weight from all perceiving handles
	 */
	UFUNCTION(BlueprintPure, Category = "LQAI|Perception|Targeting")
	float GetTargetWeight(AActor* Target) const;

	/**
	 * Gets the current target (highest weight, or locked target)
	 * @return	Current target actor, or nullptr
	 */
	UFUNCTION(BlueprintCallable, Category = "LQAI|Perception|Targeting")
	AActor* GetCurrentTarget() const;

	// === Target Lock ===

	/**
	 * Enables/disables target lock (prevents auto-switching to higher weight targets)
	 * @param	bEnabled	Whether to enable target lock
	 */
	UFUNCTION(BlueprintCallable, Category = "LQAI|Perception|Targeting")
	void SetTargetLockEnabled(bool bEnabled);

	/**
	 * Returns true if target lock is enabled
	 * @return	true if target is locked
	 */
	UFUNCTION(BlueprintPure, Category = "LQAI|Perception|Targeting")
	bool IsTargetLocked() const;

	/**
	 * Manually sets the current target (also enables lock)
	 * @param	NewTarget	The actor to set as current target
	 */
	UFUNCTION(BlueprintCallable, Category = "LQAI|Perception|Targeting")
	void SetCurrentTarget(AActor* NewTarget);

	// Clears the current target and disables lock
	UFUNCTION(BlueprintCallable, Category = "LQAI|Perception|Targeting")
	void ClearCurrentTarget();

	// === Debug Visualization ===

	/**
	 * Draws debug info at owner location showing current target and weight
	 * @param	Duration	How long to display (0 = one frame)
	 */
	UFUNCTION(BlueprintCallable, Category = "LQAI|Perception|Debug")
	void DrawDebugPerception(float Duration = 0.0f) const;

	// === Delegates ===

	// Fired when perception is gained for a handle (first time this handle perceives this target)
	UPROPERTY(BlueprintAssignable, Category = "LQAI|Perception")
	FOnLQAIPerceptionChanged OnPerceptionGained;

	// Fired when perception is lost for a handle
	UPROPERTY(BlueprintAssignable, Category = "LQAI|Perception")
	FOnLQAIPerceptionChanged OnPerceptionLost;

	// Fired when current target changes (due to weight change or lock/unlock)
	UPROPERTY(BlueprintAssignable, Category = "LQAI|Perception")
	FOnLQAICurrentTargetChanged OnCurrentTargetChanged;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Handles configured in editor (copied to runtime on BeginPlay)
	UPROPERTY(EditAnywhere, Category = "LQAI|Perception")
	TArray<TInstancedStruct<FLQAIPerceptionHandle>> DefaultHandles;

private:
	// Builds perception context
	FLQAIPerceptionContext BuildContext() const;

	// Updates current target based on weight (respects lock)
	void UpdateCurrentTarget();

	// Cleans up stale entries from PerceivedActors map
	void CleanupStalePerceptions();

	// Gets the base weight for a handle by tag
	float GetHandleWeight(FGameplayTag HandleTag) const;

	// Runtime handle array
	UPROPERTY()
	TArray<TInstancedStruct<FLQAIPerceptionHandle>> RuntimeHandles;

	// All currently perceived actors with aggregated scores
	// Key: Actor, Value: Score with per-handle weight breakdown
	TMap<TWeakObjectPtr<AActor>, FLQAIPerceptionScore> PerceivedActors;

	// Current target (highest weight or locked)
	UPROPERTY()
	TWeakObjectPtr<AActor> CurrentTarget;

	// Whether target lock is enabled
	bool bTargetLocked = false;

	// Cached context (rebuilt each tick)
	FLQAIPerceptionContext CachedContext;
};
