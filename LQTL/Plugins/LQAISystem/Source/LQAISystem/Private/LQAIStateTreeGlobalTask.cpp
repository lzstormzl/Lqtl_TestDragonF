// Copyright Tale Weavers


#include "LQAIStateTreeGlobalTask.h"
#include "GoalGenerator/LQAIGoalGenerator.h"

const UStruct* FLQAIStateTreeTask_GoalGenerator::GetInstanceDataType() const
{
	return FInstanceDataType::StaticStruct();
}

EStateTreeRunStatus FLQAIStateTreeTask_GoalGenerator::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);
	if (InstanceData.GoalGenerators.Num() == 0)
	{
		return EStateTreeRunStatus::Stopped;
	}
	for (ULQAIGoalGenerator* GoalGenIter : InstanceData.GoalGenerators)
	{
		if (GoalGenIter)
		{
			GoalGenIter->StartGenerate(Context);
		}
	}
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FLQAIStateTreeTask_GoalGenerator::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	GenerateGoals(Context, DeltaTime);
	return EStateTreeRunStatus::Running;
}

void FLQAIStateTreeTask_GoalGenerator::GenerateGoals(FStateTreeExecutionContext& Context, float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);
	for (ULQAIGoalGenerator* GoalGenIter : InstanceData.GoalGenerators)
	{
		if (GoalGenIter)
		{
			GoalGenIter->Generate(Context, DeltaTime);
		}
	}
}
