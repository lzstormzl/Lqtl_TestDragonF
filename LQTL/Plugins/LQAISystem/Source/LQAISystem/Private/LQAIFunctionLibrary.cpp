// Copyright Tale Weavers


#include "LQAIFunctionLibrary.h"
#include "LQAIInterface.h"
#include "LQAIStateTreeComponent.h"

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
