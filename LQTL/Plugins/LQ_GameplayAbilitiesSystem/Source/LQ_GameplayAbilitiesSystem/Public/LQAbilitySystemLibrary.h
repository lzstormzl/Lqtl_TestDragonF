// Copyright Tale Weavers

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "StructUtils/InstancedStruct.h"
#include "LQAbilitySystemLibrary.generated.h"

struct FGameplayAbilitySpecHandle;
struct FGameplayEffectContextHandle;
/**
 * 
 */
UCLASS()
class LQ_GAMEPLAYABILITIESSYSTEM_API ULQAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static bool TryActivateAbilityWithPayload(UAbilitySystemComponent* ASC, TSubclassOf<class UGameplayAbility> AbilityClass,
	                                          TInstancedStruct<FLQAbilityPayloadDataBase> PayloadData);

	UFUNCTION(BlueprintPure, Category="LQ|AbilitySystemLibrary")
	static void GetContextHandleFromEffectSpec(const FGameplayEffectSpec& Spec, FGameplayEffectContextHandle& OutHandle);

	UFUNCTION(BlueprintPure, Category="LQ|AbilitySystemLibrary")
	static void GetAbilitySystemActorInfo(UAbilitySystemComponent* InASC, AActor*& OwningActor, AActor*& AvatarActor);

	UFUNCTION(BlueprintCallable)
	static bool TryActivateAbilityWithPayloadAndOutHandle(UAbilitySystemComponent* ASC, TSubclassOf<class UGameplayAbility> AbilityClass, TInstancedStruct<FLQAbilityPayloadDataBase> PayloadData, FGameplayAbilitySpecHandle& OutSpecHandle);

	
	static FGameplayAbilitySpecHandle GetAbilitySpecHandleFromMontage(ULQAbilitySystemComponent* ASC, class UAnimMontage* InMontage);

	UFUNCTION(BlueprintPure)
	static ULQAbilitySystemComponent* GetLQASCFromActor(AActor* Actor);
};
