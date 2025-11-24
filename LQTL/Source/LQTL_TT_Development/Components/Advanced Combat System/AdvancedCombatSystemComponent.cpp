#include "AdvancedCombatSystemComponent.h"

void UAdvancedCombatSystemComponent::ApplyStatusEffect(
	IN AActor* _Initigator, 
	IN TSubclassOf<UCombatStatusEffectBase> _Effect, 
	IN float _Level, 
	OUT FAppliedCombatEffectHandle& _OutHandle)
{
	AppliedEffectHandler->ApplyStatusEffect(_Initigator, this->GetOwner(), _Effect, _Level, _OutHandle);
}