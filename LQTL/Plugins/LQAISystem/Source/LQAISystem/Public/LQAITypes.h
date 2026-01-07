// Copyright Tale Weavers

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "LQAITypes.generated.h"

class UStateTree;
class UBehaviorTree;

DECLARE_LOG_CATEGORY_EXTERN(LogAI, Log, All);

namespace LQAI::Goal
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Idle);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Patrol);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Retreat);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SpiritBreak);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SoloCombat);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GroupCombat);
}

namespace LQAI::StateTree
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(NonCombatStateTree);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(CombatStateTree);
}

namespace LQAI::BehaviorTree
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Idle);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SingleCombat);
}

USTRUCT(BlueprintType, meta = (DisplayName = "Goal Priority Data"))
struct FLQAIGoalPriorityData
{
	GENERATED_BODY()

	FLQAIGoalPriorityData() { }
	FLQAIGoalPriorityData(FGameplayTag InGoalTag, int32 InPriority) : GoalTag(InGoalTag), Priority(InPriority) { }

	bool operator!=(const FLQAIGoalPriorityData& Other) const
	{
		return (!GoalTag.MatchesTagExact(Other.GoalTag) || Priority != Other.Priority);
	}

	bool operator==(const FLQAIGoalPriorityData& Other) const
	{
		return (GoalTag.MatchesTagExact(Other.GoalTag) && Priority == Other.Priority);
	}

	bool operator>(const FLQAIGoalPriorityData& Other) const
	{
		return Priority > Other.Priority;
	}

	bool operator<(const FLQAIGoalPriorityData& Other) const
	{
		return Priority < Other.Priority;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Categories = "LQ.AI.Goal"))
	FGameplayTag GoalTag {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Priority = 0;

	// More to add here
};

/**
 * Struct holding loaded AI assets from the definition data asset
 * Caches loaded State Trees and Behavior Trees for quick access
 */
USTRUCT()
struct FLQAILoadedData
{
	GENERATED_BODY()

	FLQAILoadedData() { }

	/** Loaded non-combat core State Tree */
	UPROPERTY(Transient)
	TObjectPtr<UStateTree> NonCombatCoreStateTree = nullptr;

	/** Loaded combat core State Tree */
	UPROPERTY(Transient)
	TObjectPtr<UStateTree> CombatCoreStateTree = nullptr;

	/** Loaded additional State Trees mapped by gameplay tags */
	UPROPERTY(Transient, meta = (Categories = "LQ.AI.StateTree"))
	TMap<FGameplayTag, TObjectPtr<UStateTree>> AdditionalStateTreeMap;

	/** Loaded Behavior Trees mapped by gameplay tags */
	UPROPERTY(Transient, meta = (Categories = "LQ.AI.BehaviorTree"))
	TMap<FGameplayTag, TObjectPtr<UBehaviorTree>> BehaviorTreeMap;
};