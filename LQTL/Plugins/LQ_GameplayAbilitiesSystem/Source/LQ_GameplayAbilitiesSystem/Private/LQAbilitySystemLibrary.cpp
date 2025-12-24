// Copyright Tale Weavers


#include "LQAbilitySystemLibrary.h"

#include "LQAbilitySystemComponent.h"

bool ULQAbilitySystemLibrary::TryActivateAbilityWithPayload(UAbilitySystemComponent* ASC, TSubclassOf<class UGameplayAbility> AbilityClass, TInstancedStruct<FLQAbilityPayloadDataBase> PayloadData)
{
	if (auto LQASC = Cast<ULQAbilitySystemComponent>(ASC))
	{
		FGameplayAbilitySpec Spec;
		auto TryFindSpec = LQASC->FindAbilitySpecFromClass(AbilityClass);
		if (!TryFindSpec)
		{
			Spec = LQASC->BuildAbilitySpecFromClass(AbilityClass);
			LQASC->GiveAbility(Spec);
		}
		else
		{
			Spec = *TryFindSpec;
		}

		LQASC->PushAbilityPayload(Spec.Handle, PayloadData);
		return LQASC->TryActivateAbility(Spec.Handle);
	}
	return false;
}
