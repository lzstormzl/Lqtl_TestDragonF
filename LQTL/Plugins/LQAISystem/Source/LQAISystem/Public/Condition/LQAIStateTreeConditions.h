// Copyright Tale Weavers

#pragma once

#include "CoreMinimal.h"
#include "StateTreeConditionBase.h"
#include "LQAIStateTreeConditions.generated.h"

USTRUCT(BlueprintType)
struct FLQAIHasGoalConditionInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	bool bInvert = false;

	UPROPERTY(EditAnywhere, meta = (Categories = "LQ.AI.Goal"))
	FGameplayTag GoalTag {};
};

USTRUCT(DisplayName = "Has Highest Priority Goal", Category = "LQ AI")
struct FLQAIStateTreeCondition_HasHighestPriorityGoal : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FLQAIHasGoalConditionInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
};