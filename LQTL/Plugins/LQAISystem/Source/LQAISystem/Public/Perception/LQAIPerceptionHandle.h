// Copyright Tale Weavers

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InstancedStruct.h"
#include "NativeGameplayTags.h"
#include "Perception/LQPerceptionTypes.h"
#include "LQAIPerceptionHandle.generated.h"

namespace LQAI::Perception
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Range);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sight);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag);
}

/**
 * Base struct for perception handles - derive to create new perception types
 * Handles are responsible for detecting targets and reporting to the component
 */
USTRUCT(BlueprintType)
struct LQAISYSTEM_API FLQAIPerceptionHandle
{
	GENERATED_BODY()

	virtual ~FLQAIPerceptionHandle() = default;

	// Tag identifying this handle instance
	UPROPERTY(BlueprintReadOnly, Category = "Perception", meta = (Categories = "LQ.AI.Perception.Handle"))
	FGameplayTag HandleTag;

	// Whether this handle is currently enabled
	UPROPERTY(BlueprintReadWrite, Category = "Perception")
	bool bEnabled = true;

	// Base weight contribution when this handle perceives a target
	// Higher weight = more important for target selection
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception|Weight", meta = (ClampMin = "0.0"))
	float Weight = 1.0f;

	// How long (in seconds) to remember a perceived target before forgetting it if not perceived again
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception", meta = (ClampMin = "0.0"))
	float PerceptionMemoryDuration = 5.0f;

	/**
	 * Called when handle is added/activated - bind events, setup state
	 * @param	Context		Perception context containing component reference
	 */
	virtual void StartPerception(const FLQAIPerceptionContext& Context);

	/**
	 * Called each tick (if handle needs periodic updates)
	 * Tick-driven handles check for targets and call ReportPerception
	 * @param	Context		Perception context containing component reference
	 * @param	DeltaTime	Time since last tick
	 */
	virtual void TickPerception(const FLQAIPerceptionContext& Context, float DeltaTime);

	/**
	 * Called when handle is removed/deactivated - unbind events, cleanup
	 * @param	Context		Perception context containing component reference
	 */
	virtual void EndPerception(const FLQAIPerceptionContext& Context);

	/**
	 * Returns weight modifier (0.0-1.0) based on perception quality
	 * Called when reporting perception to compute effective weight
	 * @param	Target		The actor being perceived
	 * @param	Context		Perception context containing component reference
	 * @return	Weight modifier between 0.0 and 1.0
	 */
	virtual float GetWeightModifier(AActor* Target, const FLQAIPerceptionContext& Context) const;

	/**
	 * Computes effective weight: Weight * GetWeightModifier
	 * @param	Target		The actor being perceived
	 * @param	Context		Perception context containing component reference
	 * @return	Effective weight for target selection
	 */
	float ComputeEffectiveWeight(AActor* Target, const FLQAIPerceptionContext& Context) const;

	// Resets runtime state
	virtual void Reset();

	// Returns type name for debugging
	virtual FString GetDebugName() const;

	/**
	 * Draw debug visualization for this handle
	 * @param	Context		Perception context containing component reference
	 * @param	Duration	How long to display (0 = one frame)
	 */
	virtual void DrawDebug(const FLQAIPerceptionContext& Context, float Duration) const;
};

/**
 * Perception handle that checks if targets are within range
 * Tick-driven: scans for targets each tick
 */
USTRUCT(BlueprintType)
struct LQAISYSTEM_API FLQAIPerceptionHandle_Range : public FLQAIPerceptionHandle
{
	GENERATED_BODY()

	FLQAIPerceptionHandle_Range();

	// Maximum detection distance
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception|Range", meta = (ClampMin = "0.0"))
	float DetectionRadius = 1000.0f;

	// Minimum detection distance (for "not too close" checks)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception|Range", meta = (ClampMin = "0.0"))
	float MinDistance = 0.0f;

	// If true, requires unobstructed line to target
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception|Range")
	bool bRequireLineOfSight = false;

	// Trace channel for line of sight checks
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception|Range", meta = (EditCondition = "bRequireLineOfSight"))
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

	// Actor classes to scan for (empty = scan all)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception|Range")
	TArray<TSubclassOf<AActor>> TargetClasses;

	// Whether to draw debug visualization during perception checks
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception|Range|Debug")
	bool bDrawDebugPerception = false;

	// Currently perceived targets (for clearing when lost)
	mutable TSet<TWeakObjectPtr<AActor>> CurrentlyPerceivedTargets;

	// Tracks last perception time for each target (for memory-based forgetting)
	mutable TMap<TWeakObjectPtr<AActor>, float> TargetLastPerceptionTime;

	virtual void TickPerception(const FLQAIPerceptionContext& Context, float DeltaTime) override;

	/**
	 * Weight modifier: closer targets get higher weight (1.0 at MinDistance, 0.0 at DetectionRadius)
	 * @param	Target		The actor being perceived
	 * @param	Context		Perception context containing component reference
	 * @return	Weight modifier based on distance
	 */
	virtual float GetWeightModifier(AActor* Target, const FLQAIPerceptionContext& Context) const override;

	virtual void Reset() override;
	virtual FString GetDebugName() const override;
	virtual void DrawDebug(const FLQAIPerceptionContext& Context, float Duration) const override;

private:
	/**
	 * Checks if target passes perception test
	 * @param	Target		The actor to check
	 * @param	Context		Perception context containing component reference
	 * @return	true if target is perceived, false otherwise
	 */
	bool CheckTarget(AActor* Target, const FLQAIPerceptionContext& Context) const;
};

/**
 * Perception handle that checks cone-based sight with line of sight
 * Tick-driven: scans for targets within sight cone each tick
 */
USTRUCT(BlueprintType)
struct LQAISYSTEM_API FLQAIPerceptionHandle_Sight : public FLQAIPerceptionHandle
{
	GENERATED_BODY()

	FLQAIPerceptionHandle_Sight();

	// Maximum sight distance
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception|Sight", meta = (ClampMin = "0.0"))
	float SightRadius = 2000.0f;

	// Half-angle of sight cone in degrees
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception|Sight", meta = (ClampMin = "0.0", ClampMax = "180.0"))
	float SightHalfAngle = 60.0f;

	// Trace channel for visibility checks
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception|Sight")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

	// If true, uses owner's eye location instead of actor location
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception|Sight")
	bool bUseEyeLocation = true;

	// Actor classes to scan for (empty = scan all)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception|Sight")
	TArray<TSubclassOf<AActor>> TargetClasses;

	// === Runtime State ===

	// Currently perceived targets (for clearing when lost)
	mutable TSet<TWeakObjectPtr<AActor>> CurrentlyPerceivedTargets;

	// Tracks last perception time for each target (for memory-based forgetting)
	mutable TMap<TWeakObjectPtr<AActor>, float> TargetLastPerceptionTime;

	virtual void TickPerception(const FLQAIPerceptionContext& Context, float DeltaTime) override;

	/**
	 * Weight modifier: combines distance factor and angle factor
	 * Closer + more centered targets get higher weight
	 * @param	Target		The actor being perceived
	 * @param	Context		Perception context containing component reference
	 * @return	Weight modifier based on distance and angle
	 */
	virtual float GetWeightModifier(AActor* Target, const FLQAIPerceptionContext& Context) const override;

	virtual void Reset() override;
	virtual FString GetDebugName() const override;
	virtual void DrawDebug(const FLQAIPerceptionContext& Context, float Duration) const override;

private:
	/**
	 * Checks if target passes perception test (in cone + has LOS)
	 * @param	Target		The actor to check
	 * @param	Context		Perception context containing component reference
	 * @param	OwnerLocation	Owner's location (eye or actor location)
	 * @param	OwnerForward	Owner's forward direction
	 * @return	true if target is perceived, false otherwise
	 */
	bool CheckTarget(AActor* Target, const FLQAIPerceptionContext& Context, const FVector& OwnerLocation, const FVector& OwnerForward) const;

	/**
	 * Gets owner's eye location, or actor location if eye location not available
	 * @param	Owner		The owner actor
	 * @return	Eye location or actor location
	 */
	FVector GetOwnerEyeLocation(AActor* Owner) const;
};

/**
 * Perception handle that detects actors with specific gameplay tags
 * Event-driven: can tick to scan for tag-matching actors
 */
USTRUCT(BlueprintType)
struct LQAISYSTEM_API FLQAIPerceptionHandle_Tag : public FLQAIPerceptionHandle
{
	GENERATED_BODY()

	FLQAIPerceptionHandle_Tag();

	// Tag query that target must match
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception|Tag")
	FGameplayTagQuery RequiredTagQuery;

	// If true, also requires target to be within range
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception|Tag")
	bool bRequireRange = false;

	// Range requirement (if bRequireRange is true)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception|Tag", meta = (EditCondition = "bRequireRange", ClampMin = "0.0"))
	float MaxRange = 1000.0f;

	// Actor classes to scan for (empty = scan all)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception|Tag")
	TArray<TSubclassOf<AActor>> TargetClasses;

	// Currently perceived targets (for clearing when lost)
	mutable TSet<TWeakObjectPtr<AActor>> CurrentlyPerceivedTargets;

	// Tracks last perception time for each target (for memory-based forgetting)
	mutable TMap<TWeakObjectPtr<AActor>, float> TargetLastPerceptionTime;

	virtual void StartPerception(const FLQAIPerceptionContext& Context) override;
	virtual void TickPerception(const FLQAIPerceptionContext& Context, float DeltaTime) override;
	virtual void EndPerception(const FLQAIPerceptionContext& Context) override;

	/**
	 * Weight modifier: 1.0 for tag matches (binary perception)
	 * Can be scaled by range if bRequireRange is true
	 * @param	Target		The actor being perceived
	 * @param	Context		Perception context containing component reference
	 * @return	Weight modifier (1.0 if tags match, 0.0 otherwise)
	 */
	virtual float GetWeightModifier(AActor* Target, const FLQAIPerceptionContext& Context) const override;

	virtual void Reset() override;
	virtual FString GetDebugName() const override;
	virtual void DrawDebug(const FLQAIPerceptionContext& Context, float Duration) const override;

private:
	/**
	 * Checks if target has required tags
	 * @param	Target		The actor to check
	 * @return	true if target matches tag query
	 */
	bool CheckTargetTags(AActor* Target) const;

	/**
	 * Checks if target passes all perception tests (tags + optional range)
	 * @param	Target		The actor to check
	 * @param	Context		Perception context containing component reference
	 * @return	true if target is perceived, false otherwise
	 */
	bool CheckTarget(AActor* Target, const FLQAIPerceptionContext& Context) const;
};

