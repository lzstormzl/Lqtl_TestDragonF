#include "ActionEvaluations.h"

#include "GameFramework/Pawn.h"
#include "EnhancedInputComponent.h"

void UActionEvaluationInputAction::Evaluate(const AActor* _Owner) {
	const APawn* asPawn = Cast<APawn>(_Owner);
	
	if (asPawn == nullptr) return;

    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(asPawn->InputComponent))
    {
        if (InputAction != nullptr)
        {
            EIC->BindAction(InputAction, ETriggerEvent::Triggered, this, &UActionEvaluationInputAction::OnInputTriggered);
        }
    }
}

void UActionEvaluationInputAction::OnInputTriggered() {

}