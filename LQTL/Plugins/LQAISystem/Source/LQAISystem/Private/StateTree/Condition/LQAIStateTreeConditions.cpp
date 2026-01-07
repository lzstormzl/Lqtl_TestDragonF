// Copyright Tale Weavers


#include "StateTree/Condition/LQAIStateTreeConditions.h"
#include "StateTreeExecutionContext.h"
#include "LQAIFunctionLibrary.h"
#include "LQAITypes.h"
#include "StateTree/LQAIStateTreeComponent.h"
#include "GameFramework/Actor.h"

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
