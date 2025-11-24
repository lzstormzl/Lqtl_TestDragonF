#pragma once

#include "CoreMinimal.h"

#include "ActionEvaluationBase.generated.h"

/*
Base class for generic actions evaluation trigger (e.g. Attack combo, Conditional interactions, etc.)
*/
UCLASS(Abstract, NotBlueprintable, EditInlineNew)
class LQTL_TT_DEVELOPMENT_API UActionEvaluationBase : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual void Evaluate(const AActor* _Owner) { }
};