// Copyright Tale Weavers

#pragma once

#include "CoreMinimal.h"
#include "StateTreeEvaluatorBase.h"
#include "StateTreeReference.h"
#include "StateTreeExecutionContext.h"
#include "GameplayTagContainer.h"
#include "LQAIStateTreeEvaluators.generated.h"

class UBehaviorTree;

/**
 * Instance data for getting an additional State Tree by gameplay tag
 */
USTRUCT(BlueprintType)
struct FLQAIGetAdditionalStateTreeInstanceData
{
	GENERATED_BODY()

	/** Output State Tree reference retrieved from the additional map */
	UPROPERTY(EditAnywhere, meta = (Output))
	FStateTreeReference StateTree;

	/** Gameplay tag used to look up the State Tree in the additional map */
	UPROPERTY(EditAnywhere, meta = (Categories = "LQ.AI.StateTree"))
	FGameplayTag StateTreeTag {};
};

/**
 * State Tree evaluator that retrieves an additional State Tree by gameplay tag from the AI definition asset
 */
USTRUCT(BlueprintType, meta = (DisplayName = "Get Additional State Tree"), Category = "LQ AI")
struct FLQAIGetAdditionalStateTreeEvaluator : public FStateTreeEvaluatorCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FLQAIGetAdditionalStateTreeInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const;
};

/**
 * Instance data for getting the non-combat core State Tree
 */
USTRUCT(BlueprintType)
struct FLQAIGetNonCombatCoreStateTreeInstanceData
{
	GENERATED_BODY()

	/** Output non-combat core State Tree reference */
	UPROPERTY(EditAnywhere, meta = (Output))
	FStateTreeReference StateTree;
};

/**
 * State Tree evaluator that retrieves the non-combat core State Tree from the AI definition asset
 */
USTRUCT(BlueprintType, meta = (DisplayName = "Get Non-Combat Core State Tree"), Category = "LQ AI")
struct FLQAIGetNonCombatCoreStateTreeEvaluator : public FStateTreeEvaluatorCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FLQAIGetNonCombatCoreStateTreeInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const;
};

/**
 * Instance data for getting the combat core State Tree
 */
USTRUCT(BlueprintType)
struct FLQAIGetCombatCoreStateTreeInstanceData
{
	GENERATED_BODY()

	/** Output combat core State Tree reference */
	UPROPERTY(EditAnywhere, meta = (Output))
	FStateTreeReference StateTree;
};

/**
 * State Tree evaluator that retrieves the combat core State Tree from the AI definition asset
 */
USTRUCT(BlueprintType, meta = (DisplayName = "Get Combat Core State Tree"), Category = "LQ AI")
struct FLQAIGetCombatCoreStateTreeEvaluator : public FStateTreeEvaluatorCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FLQAIGetCombatCoreStateTreeInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const;
};

/**
 * Instance data for getting a Behavior Tree by gameplay tag
 */
USTRUCT(BlueprintType)
struct FLQAISTEvaluator_GetBTInstanceData
{
	GENERATED_BODY()

	/** Output Behavior Tree retrieved from the map */
	UPROPERTY(EditAnywhere, meta = (Output), Category = "Output")
	UBehaviorTree* BehaviorTree = nullptr;

	/** Gameplay tag used to look up the Behavior Tree */
	UPROPERTY(EditAnywhere, meta = (Categories = "LQ.AI.BehaviorTree"), Category = "Input")
	FGameplayTag BTTag {};
};

/**
 * State Tree evaluator that retrieves a Behavior Tree by gameplay tag from the AI definition asset
 */
USTRUCT(BlueprintType, meta = (DisplayName = "Get BT"), Category = "LQ AI")
struct FLQAISTEvaluator_GetBT : public FStateTreeEvaluatorCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FLQAISTEvaluator_GetBTInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const;
};