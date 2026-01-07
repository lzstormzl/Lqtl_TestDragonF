// Copyright Tale Weavers


#include "LQAbilitySystemLibrary.h"
#include "LQAbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

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

void ULQAbilitySystemLibrary::GetContextHandleFromEffectSpec(const FGameplayEffectSpec& Spec, FGameplayEffectContextHandle& OutHandle)
{
	OutHandle = Spec.GetEffectContext();
}

void ULQAbilitySystemLibrary::GetAbilitySystemActorInfo(UAbilitySystemComponent* InASC, AActor*& OwningActor, AActor*& AvatarActor)
{
	if (InASC)
	{
		OwningActor = InASC->GetOwnerActor();
		AvatarActor = InASC->GetAvatarActor();
	}
}

bool ULQAbilitySystemLibrary::TryActivateAbilityWithPayloadAndOutHandle(UAbilitySystemComponent* ASC, TSubclassOf<class UGameplayAbility> AbilityClass, TInstancedStruct<FLQAbilityPayloadDataBase> PayloadData,
                                                                        FGameplayAbilitySpecHandle& OutSpecHandle)
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

		OutSpecHandle = Spec.Handle;
		LQASC->PushAbilityPayload(Spec.Handle, PayloadData);
		return LQASC->TryActivateAbility(Spec.Handle);
	}

	OutSpecHandle = FGameplayAbilitySpecHandle();
	return false;
}

FGameplayAbilitySpecHandle ULQAbilitySystemLibrary::GetAbilitySpecHandleFromMontage(ULQAbilitySystemComponent* ASC, class UAnimMontage* InMontage)
{
	if (ASC)
	{
		return ASC->GetAbilitySpecHandleFromMontage(InMontage);
	}
	return FGameplayAbilitySpecHandle();
}

ULQAbilitySystemComponent* ULQAbilitySystemLibrary::GetLQASCFromActor(AActor* Actor)
{
	if (IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(Actor))
	{
		return Cast<ULQAbilitySystemComponent>(ASCInterface->GetAbilitySystemComponent());
	}
	return nullptr;
}
