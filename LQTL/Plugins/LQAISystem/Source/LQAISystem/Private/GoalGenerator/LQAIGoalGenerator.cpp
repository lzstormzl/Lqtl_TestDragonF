// Copyright Tale Weavers


#include "GoalGenerator/LQAIGoalGenerator.h"
#include "LQAIFunctionLibrary.h"
#include "LQAIStateTreeComponent.h"


void ULQAIGoalGenerator::StartGenerate(FStateTreeExecutionContext& Context)
{
	
}

void ULQAIGoalGenerator::Generate(FStateTreeExecutionContext& Context, float DeltaTime)
{
	if (AActor* OwnerActor = Cast<AActor>(Context.GetOwner()))
	{
		if (ULQAIStateTreeComponent* StateTreeComp = ULQAIFunctionLibrary::GetAIStateTreeComponent(Cast<AActor>(Context.GetOwner())))
		{
			if (GenerateGoal(OwnerActor))
			{
				// Add goal here
			}
		}
	}
}

void ULQAIGoalGenerator::EndGenerate()
{
	
}

bool ULQAIGoalGenerator::GenerateGoal_Implementation(AActor* Owner)
{
	return false;
}
