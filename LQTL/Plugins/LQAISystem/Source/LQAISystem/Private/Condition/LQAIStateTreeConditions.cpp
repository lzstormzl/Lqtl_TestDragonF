// Copyright Tale Weavers


#include "Condition/LQAIStateTreeConditions.h"

bool FLQAIStateTreeCondition_HasHighestPriorityGoal::TestCondition(FStateTreeExecutionContext& Context) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (ULQAIStateTreeComponent* StateTreeComp = ULQAIFunctionLibrary::GetAIStateTreeComponent(Cast<AActor>(Context.GetOwner())))
	{
		FLQAIGoalPriorityData HighestPriorityData;
		if (StateTreeComp->GetHighestPriorityGoalData(HighestPriorityData, false))
		{
			bool bResult = HighestPriorityData.GoalTag.MatchesTag(InstanceData.GoalTag);
			return !InstanceData.bInvert ? bResult : !bResult;
		}
	}
	return false;
}
