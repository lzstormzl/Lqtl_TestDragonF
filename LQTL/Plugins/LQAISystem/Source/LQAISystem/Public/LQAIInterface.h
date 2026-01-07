// Copyright Tale Weavers

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LQAIInterface.generated.h"

class ULQAIStateTreeComponent;

UINTERFACE(MinimalAPI)
class ULQAIInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for actors that have AI functionality powered by State Trees
 * Implementing this interface allows actors to be queried for their AI State Tree component
 */
class LQAISYSTEM_API ILQAIInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface
public:
	/**
	 * Gets the AI State Tree component associated with this actor
	 * @return  The AI State Tree component managing this actor's AI behavior
	 */
	virtual ULQAIStateTreeComponent* GetAIStateTreeComponent() = 0;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	AActor* GetAICurrentTarget();
	virtual AActor* GetAICurrentTarget_Implementation();
};
