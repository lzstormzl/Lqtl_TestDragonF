// Copyright Tale Weavers

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LQAIFunctionLibrary.generated.h"

class ULQAIStateTreeComponent;

UCLASS()
class LQAISYSTEM_API ULQAIFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Retrieves the AI State Tree component from an actor.
	 * Checks if the actor implements ILQAIInterface and returns its state tree component.
	 * @param	Actor	The actor to retrieve the AI State Tree component from
	 * @return	The AI State Tree component if found, nullptr otherwise
	 */
	UFUNCTION(BlueprintPure, Category = "LQ AI")
	static ULQAIStateTreeComponent* GetAIStateTreeComponent(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "LQ AI", meta = (Categories = "LQ.AI.Goal"))
	static bool AddAIGoalForActor(AActor* Actor, FGameplayTag GoalTag);
};
