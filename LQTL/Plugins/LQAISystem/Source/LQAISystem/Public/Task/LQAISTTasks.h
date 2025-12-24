// Copyright Tale Weavers

#pragma once

#include "CoreMinimal.h"
#include <GameplayTagContainer.h>
#include "LQAISTTasks.generated.h"

USTRUCT(BlueprintType)
struct FLQAISTTask_TrackHighestPriorityGoalInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (Categories = "LQ.AI.Goal"))
	bool bInvert = false;

	UPROPERTY(EditAnywhere, meta = (Categories = "LQ.AI.Goal"))
	FGameplayTag GoalTag {};
};

USTRUCT(BlueprintType, Category = "LQ AI", meta = (DisplayName = "Track Highest Priority Goal"))
struct FLQAISTTask_TrackHighestPriorityGoal : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FLQAISTTask_TrackHighestPriorityGoalInstanceData;
	virtual const UStruct* GetInstanceDataType() const { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
};

UENUM(BlueprintType)
enum class ELQAIRunBTEndType : uint8
{
	LeaveBTRunning, 
	PauseBT, 
	StopBT,
};

USTRUCT(BlueprintType)
struct FLQAISTTask_RunBTInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (Categories = "LQ.AI.BehaviorTree"))
	FGameplayTag BTTag {};

	UPROPERTY(EditAnywhere)
	ELQAIRunBTEndType EndBTType = ELQAIRunBTEndType::StopBT;
};

USTRUCT(BlueprintType, Category = "LQ AI", meta = (DisplayName = "Run BT"))
struct FLQAISTTask_RunBT : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FLQAISTTask_RunBTInstanceData;
	virtual const UStruct* GetInstanceDataType() const { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};
