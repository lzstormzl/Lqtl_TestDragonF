// Copyright Tale Weavers

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "LQHitboxTypes.h"
#include "StructUtils/StructView.h"
#include "UObject/Object.h"
#include "LQAbilitySystemTypes.h"
#include "OnHitBehaviorBase.generated.h"

/**
 * 
 */
UCLASS(HideDropdown, BlueprintType, Blueprintable)
class LQHITBOXSYSTEM_API UOnHitBehaviorBase : public UObject
{
	GENERATED_BODY()

public:
	virtual class UWorld* GetWorld() const override;

	UFUNCTION(BlueprintNativeEvent)
	void ExecuteOnHit(AActor* Instigator, const FGameplayAbilitySpecHandle& AbilitySpecHandle, const FHitResult& TargetHitResult, const TInstancedStruct<FHitboxAttackData>& AnimHitboxData);
};
