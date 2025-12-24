// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "LQAbilitySystemTypes.h"
#include "StructUtils/InstancedStruct.h"
#include "LQAbilitySystemComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LQ_GAMEPLAYABILITIESSYSTEM_API ULQAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	friend class ULQGameplayAbility;
	friend class ULQAbilitySystemLibrary;

public:
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;

protected:
	ULQAbilitySystemComponent();
	virtual void BeginPlay() override;

	void PushAbilityPayload(const FGameplayAbilitySpecHandle& Handle, const TInstancedStruct<FLQAbilityPayloadDataBase>& PayloadData);
	bool PopAbilityPayload(const FGameplayAbilitySpecHandle& Handle, TInstancedStruct<FLQAbilityPayloadDataBase>& OutPayloadData);

private:
	TMap<FGameplayAbilitySpecHandle, TInstancedStruct<FLQAbilityPayloadDataBase>> AbilityPayloads;
};
