// Copyright Tale Weavers


#include "LQAIFunctionLibrary.h"
#include "LQAIInterface.h"
#include "LQAITypes.h"
#include "StateTree/LQAIStateTreeComponent.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "HAL/IConsoleManager.h"

ULQAIStateTreeComponent* ULQAIFunctionLibrary::GetAIStateTreeComponent(AActor* Actor)
{
	if (Actor)
	{
		if (ILQAIInterface* AIInterface = Cast<ILQAIInterface>(Actor))
		{
			return AIInterface->GetAIStateTreeComponent();
		}
		return Actor->GetComponentByClass<ULQAIStateTreeComponent>();
	}
	return nullptr;
}

bool ULQAIFunctionLibrary::AddAIGoalForActor(AActor* Actor, FGameplayTag GoalTag)
{
	if (ULQAIStateTreeComponent* AIStateTreeComp = ULQAIFunctionLibrary::GetAIStateTreeComponent(Actor))
	{
		return AIStateTreeComp->AddGoal(GoalTag);
	}
	return false;
}

UBlackboardComponent* ULQAIFunctionLibrary::GetBlackboardComponentFromActor(AActor* Actor)
{
	if (Actor)
	{
		AAIController* AIController;
		if (ACharacter* AsChar = Cast<ACharacter>(Actor))
		{
			AIController = AsChar->GetController<AAIController>();
		}
		else
		{
			AIController = Cast<AAIController>(Actor);
		}
		if (AIController)
		{
			return AIController->GetBlackboardComponent();
		}
	}
	return nullptr;
}

AActor* ULQAIFunctionLibrary::GetAICurrentTargetFromActor(AActor* Actor)
{
	if (Actor && Actor->Implements<ULQAIInterface>())
	{
		return ILQAIInterface::Execute_GetAICurrentTarget(Actor);
	}
	return nullptr;
}

bool ULQAIFunctionLibrary::IsDebugEnabled()
{
	IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("LQAI.ShowDebug"));
	return CVar && CVar->GetBool();
}
