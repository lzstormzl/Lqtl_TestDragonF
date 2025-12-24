// Copyright Tale Weavers


#include "Task/LQAISTTasks.h"
#include "LQAIStateTreeComponent.h"
#include "LQAIFunctionLibrary.h"
#include "StateTreeExecutionContext.h"
#include <BehaviorTree/BehaviorTree.h>
#include <GameFramework/Character.h>
#include <AIController.h>
#include <BrainComponent.h>

EStateTreeRunStatus FLQAISTTask_TrackHighestPriorityGoal::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	if (FInstanceDataType* InstanceData = Context.GetInstanceDataPtr<FInstanceDataType>(*this))
	{
		if (ULQAIStateTreeComponent* STComp = ULQAIFunctionLibrary::GetAIStateTreeComponent(Cast<AActor>(Context.GetOwner())))
		{
			FLQAIGoalPriorityData GoalData;
			if (STComp->GetHighestPriorityGoalData(GoalData, false))
			{
				bool bHavingTag = GoalData.GoalTag.MatchesTag(InstanceData->GoalTag);
				bool bContinue = !InstanceData->bInvert ? bHavingTag : !bHavingTag;
				if (bContinue)
				{
					return EStateTreeRunStatus::Running;
				}
				else
				{
					return EStateTreeRunStatus::Failed;
				}
			}
		}
	}
	return EStateTreeRunStatus::Failed;
}

EStateTreeRunStatus FLQAISTTask_RunBT::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	if (FInstanceDataType* InstanceData = Context.GetInstanceDataPtr<FInstanceDataType>(*this))
	{
		if (InstanceData->BTTag.IsValid())
		{
			if (ULQAIStateTreeComponent* STComp = ULQAIFunctionLibrary::GetAIStateTreeComponent(Cast<AActor>(Context.GetOwner())))
			{
				if (UBehaviorTree* FoundBT = STComp->GetBehaviorTreeByTag(InstanceData->BTTag))
				{
					if (ACharacter* OwnerChar = Cast<ACharacter>(Context.GetOwner()))
					{
						if (AAIController* AIController = OwnerChar->GetOwner<AAIController>())
						{
							bool bResult = AIController->RunBehaviorTree(FoundBT);
							return bResult ? EStateTreeRunStatus::Running : EStateTreeRunStatus::Failed;
						}
					}
				}
			}
		}
	}
	return EStateTreeRunStatus::Failed;
}

void FLQAISTTask_RunBT::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	if (FInstanceDataType* InstanceData = Context.GetInstanceDataPtr<FInstanceDataType>(*this))
	{
		if (ACharacter* OwnerChar = Cast<ACharacter>(Context.GetOwner()))
		{
			if (AAIController* AIController = OwnerChar->GetOwner<AAIController>())
			{
				if (UBrainComponent* BrainComp = AIController->GetBrainComponent())
				{
					switch (InstanceData->EndBTType)
					{
					case ELQAIRunBTEndType::StopBT:
					{
						BrainComp->StopLogic(TEXT("ST End BT"));
						break;
					}
					case ELQAIRunBTEndType::PauseBT:
					{
						BrainComp->PauseLogic(TEXT("ST Pause BT"));
						break;
					}
					}
				}
			}
		}
	}
}
