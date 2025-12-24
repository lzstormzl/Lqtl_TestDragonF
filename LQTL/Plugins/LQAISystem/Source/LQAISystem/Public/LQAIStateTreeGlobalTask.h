// Copyright Tale Weavers

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "LQAIStateTreeGlobalTask.generated.h"

class ULQAIGoalGenerator;

USTRUCT()
struct FLQAIStateTreeTask_GoalGeneratorData
{
    GENERATED_BODY()

public:
    UPROPERTY(Instanced)
	TArray<ULQAIGoalGenerator*> GoalGenerators {};
};

USTRUCT(BlueprintType, meta = (DisplayName = "Goal Generator"))
struct FLQAIStateTreeTask_GoalGenerator : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FLQAIStateTreeTask_GoalGeneratorData;

public:
	// FStateTreeTaskCommonBase override starts
	virtual const UStruct* GetInstanceDataType() const override;
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	// FStateTreeTaskCommonBase override ends

protected: 
	void GenerateGoals(FStateTreeExecutionContext& Context, float DeltaTime) const;
};