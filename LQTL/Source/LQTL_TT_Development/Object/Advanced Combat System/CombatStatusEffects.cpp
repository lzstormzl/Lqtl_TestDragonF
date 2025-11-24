#include "CombatStatusEffects.h"

void UCombatStatusEffectBase::BeginDestroy() {
	Super::BeginDestroy();

	UE_LOG(LogTemp, Warning, TEXT("Effect object destroyed: %hs"), TCHAR_TO_ANSI(*this->GetClass()->GetName()))
}

bool UCombatStatusEffectBase::OnStatusEffectApplied_Implementation(AActor* _Initigator, AActor* _Target, float _Level)
{
	return true;
}

bool UCombatStatusEffectBase::OnStatusEffectRemoved_Implementation(AActor* _Initigator, AActor* _Target, float _Level)
{
	return true;
}

bool UCombatStatusEffectBase::ApplyEffect(AActor* _Initigator, AActor* _Target, float _Level)
{
	if (_Level > 0) {
		if (!OnStatusEffectApplied_Implementation(_Initigator, _Target, _Level)) return false;
	}
	else {
		if (!OnStatusEffectRemoved_Implementation(_Initigator, _Target, _Level)) return false;
	}

	EffectLevel = FMath::Clamp(EffectLevel + _Level, MinLevel, MaxLevel);

	return true;
}



void UAppliedEffectHandler::ApplyStatusEffect(
	IN AActor* _Initigator,
	IN AActor* _Target,
	IN TSubclassOf<UCombatStatusEffectBase> _Effect,
	IN float _Level,
	OUT FAppliedCombatEffectHandle& _OutHandle)
{
	_OutHandle.AppliedEffectClass = _Effect;
	_OutHandle.Initigator = _Initigator;
	_OutHandle.Target = _Target;

	// Modify (or add) new levels to the given effect.
	auto findResult = AppliedEffects.Find(_Effect);
	TObjectPtr<UCombatStatusEffectBase> effectPtr;
	if (findResult == nullptr) {
		if (_Level < 0) return;

		effectPtr = AppliedEffects.Add(_Effect);

		OnEffectAdded.Broadcast(_Effect);
	}
	else {
		effectPtr = *findResult;
	}

	float orgLevel = effectPtr->GetCurrentEffectLevel();
	effectPtr->ApplyEffect(_Initigator, _Target, _Level);

	// Check if effect was removed.
	if (effectPtr->GetCurrentEffectLevel() <= effectPtr->GetMinLevel()) {

		// CC001: Once removed from the applied list of effects, the object containing the effect it self *SHOULD* be removed automatically by 
		// the garbage collector. If not, object destroy should be explicitly handled here.

		AppliedEffects.Remove(_Effect);

		OnEffectRemoved.Broadcast(_Effect);
	}

	_OutHandle.AppliedLevel = effectPtr->GetCurrentEffectLevel() - orgLevel;
	
	OnEffectModified.Broadcast(_OutHandle);
}

void UAppliedEffectHandler::RevertEffectApplication(const FAppliedCombatEffectHandle& _Handle)
{
	if (_Handle.AppliedLevel == 0) return;
	
	FAppliedCombatEffectHandle Temp;
	ApplyStatusEffect(_Handle.Initigator, _Handle.Target, _Handle.AppliedEffectClass, -_Handle.AppliedLevel, Temp);
}
