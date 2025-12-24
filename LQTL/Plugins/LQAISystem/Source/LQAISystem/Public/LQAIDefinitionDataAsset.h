// Copyright Tale Weavers

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include <GameplayTagContainer.h>
#include "LQAITypes.h"
#include "LQAIDefinitionDataAsset.generated.h"

class UStateTree;
class UBehaviorTree;

/**
 * Primary data asset defining AI behavior configuration
 * Contains State Tree and Behavior Tree mappings along with goal priorities
 */
UCLASS()
class LQAISYSTEM_API ULQAIDefinitionDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** Core State Tree used for non-combat AI state tree */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LQ AI : State Tree", meta = (AssetBundles = "Core"))
	TSoftObjectPtr<UStateTree> NonCombatCoreStateTree = nullptr;

	/** Core State Tree used for combat AI state tree */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LQ AI : State Tree", meta = (AssetBundles = "Core"))
	TSoftObjectPtr<UStateTree> CombatCoreStateTree = nullptr;

	/** Maps gameplay tags to State Tree assets for different AI states */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LQ AI : State Tree", meta = (Categories = "LQ.AI.StateTree", AssetBundles = "Core"))
	TMap<FGameplayTag, TSoftObjectPtr<UStateTree>> AdditionalTagToStateTreeMap {};

	/** Defines priority values for different AI goals */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LQ AI : Goal", meta = (Categories = "LQ.AI.Goals"))
	TArray<FLQAIGoalPriorityData> GoalDataset
	{
		{ LQAI::Goal::Idle, 0 },
		{ LQAI::Goal::SoloCombat, 50 },
		{ LQAI::Goal::SpiritBreak, 100 },
	};

	/** Default goals assigned to AI on initialization */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LQ AI : Goal", meta = (Categories = "LQ.AI.Goals"))
	TArray<FGameplayTag> DefaultGoals
	{
		LQAI::Goal::Idle
	};

	/** Maps gameplay tags to Behavior Tree assets for different AI behaviors */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LQ AI : Behavior Tree", meta = (Categories = "LQ.AI.BehaviorTree", AssetBundles = "Core"))
	TMap<FGameplayTag, TSoftObjectPtr<UBehaviorTree>> TagToBTMap
	{
		{LQAI::BehaviorTree::Idle, nullptr}, {LQAI::BehaviorTree::SingleCombat, nullptr}
	};
};
