#pragma once

#include "CoreMinimal.h"

#include "ActionEvaluationBase.h"
#include "InputAction.h"

#include "ActionEvaluations.generated.h"

UCLASS()
class UActionEvaluationInputAction : public UActionEvaluationBase
{
	GENERATED_BODY()

public: 
	UPROPERTY(Instanced)
	const UInputAction* InputAction;

	UPROPERTY()
	ETriggerEvent TriggerEvent;

	virtual void Evaluate(const AActor* _Owner) override;

	void OnInputTriggered();
};