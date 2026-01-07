// Copyright Tale Weavers

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "LQPerceptionTypes.generated.h"

// Forward declarations
class ULQAIPerceptionComponent;

// Console variables
extern bool bDrawDebugPerception;

// ============================================================================
// CONTEXT STRUCT (Minimal - handles retrieve data from component)
// ============================================================================

/**
 * Context passed to perception handles
 * Handles can retrieve data from the perception component reference
 */
USTRUCT(BlueprintType)
struct LQAISYSTEM_API FLQAIPerceptionContext
{
	GENERATED_BODY()

	// Reference to the owning perception component
	// Handles can get Owner, World, etc. from this
	UPROPERTY()
	TWeakObjectPtr<ULQAIPerceptionComponent> PerceptionComponent;

	// Returns true if context is valid
	bool IsValid() const;

	// Helper to get owner actor
	AActor* GetOwner() const;

	// Helper to get world
	UWorld* GetWorld() const;
};

// ============================================================================
// PERCEPTION SCORE (Per-Actor Aggregation)
// ============================================================================

/**
 * Aggregated perception score for a single actor across all handles
 * Used as value in TMap<AActor*, FLQAIPerceptionScore>
 */
USTRUCT(BlueprintType)
struct LQAISYSTEM_API FLQAIPerceptionScore
{
	GENERATED_BODY()

	// Total weight = sum of (Handle.Weight * WeightModifier) for all perceiving handles
	UPROPERTY(BlueprintReadOnly, Category = "Perception")
	float TotalWeight = 0.0f;

	// Per-handle weight contributions (HandleTag -> Weight * Modifier)
	// Allows updating individual handle contributions without full recalc
	TMap<FGameplayTag, float> HandleWeights;

	// Which handles currently perceive this actor
	UPROPERTY(BlueprintReadOnly, Category = "Perception")
	TArray<FGameplayTag> PerceivingHandles;

	// Whether any perception is retained (vs all direct)
	UPROPERTY(BlueprintReadOnly, Category = "Perception")
	bool bAnyRetained = false;

	// Recalculates TotalWeight from HandleWeights
	void RecalculateTotalWeight();

	// Comparison for sorting (higher weight first)
	bool operator>(const FLQAIPerceptionScore& Other) const;
};

