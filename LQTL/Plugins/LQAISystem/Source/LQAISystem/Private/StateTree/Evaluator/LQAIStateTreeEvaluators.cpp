// Copyright Tale Weavers


#include "StateTree/Evaluator/LQAIStateTreeEvaluators.h"
#include "StateTreeExecutionContext.h"
#include "LQAIFunctionLibrary.h"
#include "StateTree/LQAIStateTreeComponent.h"
#include "GameFramework/Actor.h"

void FLQAIGetAdditionalStateTreeEvaluator::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	if (FInstanceDataType* InstanceData = Context.GetInstanceDataPtr<FInstanceDataType>(*this))
	{
		if (InstanceData->StateTreeTag.IsValid())
		{
			if (ULQAIStateTreeComponent* STComp = ULQAIFunctionLibrary::GetAIStateTreeComponent(Cast<AActor>(Context.GetOwner())))
			{
				InstanceData->StateTree.SetStateTree(STComp->GetAdditionalStateTreeByTag(InstanceData->StateTreeTag));
			}
		}
	}
}

void FLQAIGetNonCombatCoreStateTreeEvaluator::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	if (FInstanceDataType* InstanceData = Context.GetInstanceDataPtr<FInstanceDataType>(*this))
	{
		if (ULQAIStateTreeComponent* STComp = ULQAIFunctionLibrary::GetAIStateTreeComponent(Cast<AActor>(Context.GetOwner())))
		{
			InstanceData->StateTree.SetStateTree(STComp->GetNonCombatCoreStateTree());
		}
	}
}

void FLQAIGetCombatCoreStateTreeEvaluator::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	if (FInstanceDataType* InstanceData = Context.GetInstanceDataPtr<FInstanceDataType>(*this))
	{
		if (ULQAIStateTreeComponent* STComp = ULQAIFunctionLibrary::GetAIStateTreeComponent(Cast<AActor>(Context.GetOwner())))
		{
			InstanceData->StateTree.SetStateTree(STComp->GetCombatCoreStateTree());
		}
	}
}

void FLQAISTEvaluator_GetBT::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	if (FInstanceDataType* InstanceData = Context.GetInstanceDataPtr<FInstanceDataType>(*this))
	{
		if (InstanceData->BTTag.IsValid())
		{
			if (ULQAIStateTreeComponent* STComp = ULQAIFunctionLibrary::GetAIStateTreeComponent(Cast<AActor>(Context.GetOwner())))
			{
				InstanceData->BehaviorTree = STComp->GetBehaviorTreeByTag(InstanceData->BTTag);
			}
		}
	}
}
