// Fill out your copyright notice in the Description page of Project Settings.


#include "LQAbilitySystemComponent.h"

#include "LQAbilitySystemInterface.h"


// Sets default values for this component's properties
ULQAbilitySystemComponent::ULQAbilitySystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void ULQAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();
}

void ULQAbilitySystemComponent::PushAbilityPayload(const FGameplayAbilitySpecHandle& Handle,
                                                   const TInstancedStruct<FLQAbilityPayloadDataBase>& PayloadData)
{
	if (AbilityPayloads.Contains(Handle))
	{
		AbilityPayloads[Handle] = PayloadData;
	}
	else
	{
		AbilityPayloads.Add(Handle, PayloadData);
	}
}

bool ULQAbilitySystemComponent::PopAbilityPayload(const FGameplayAbilitySpecHandle& Handle,
                                                  TInstancedStruct<FLQAbilityPayloadDataBase>& OutPayloadData)
{
	if (TInstancedStruct<FLQAbilityPayloadDataBase>* FoundPayload = AbilityPayloads.Find(Handle))
	{
		OutPayloadData = *FoundPayload;
		AbilityPayloads.Remove(Handle);
		return true;
	}
	return false;
}

void ULQAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);
	if (ILQAbilitySystemInterface* OwnerInterface = Cast<ILQAbilitySystemInterface>(InOwnerActor))
	{
		OwnerInterface->OnAbilitySystemInitialized();
	}
	if (ILQAbilitySystemInterface* AvatarInterface = Cast<ILQAbilitySystemInterface>(InAvatarActor))
	{
		AvatarInterface->OnAbilitySystemInitialized();
	}
}
