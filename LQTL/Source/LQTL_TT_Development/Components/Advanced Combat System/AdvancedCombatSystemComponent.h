#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "LQTL_TT_Development/Object/Advanced Combat System/CombatAttribute.h"
#include "LQTL_TT_Development/Object/Advanced Combat System/CombatStatusEffects.h"

#include "AdvancedCombatSystemComponent.generated.h"

/*
Core component for handling combat effects and attributes.
*/
UCLASS(BlueprintType, ClassGroup = AdvancedCombatSystem, meta = (BlueprintSpawnableComponent))
class LQTL_TT_DEVELOPMENT_API UAdvancedCombatSystemComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UAppliedEffectHandler* AppliedEffectHandler;

public:
	// UAttributeSet contains a number of attributes to be used in with this combat unit
	// New attribute sets can be created from UAttributeSet
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Instanced, Category = "Attribute")
	TObjectPtr<UCombatAttributeSet> AttributeSet;

	// Apply a status effect to this component
	// Internal logic are handled through UAppliedEffectHandler
	UFUNCTION(BlueprintCallable, Category = "Combat|Effect")
	void ApplyStatusEffect(
		AActor* _Initigator,
		TSubclassOf<UCombatStatusEffectBase> _Effect,
		float _Level,
		FAppliedCombatEffectHandle& _OutHandle);
};