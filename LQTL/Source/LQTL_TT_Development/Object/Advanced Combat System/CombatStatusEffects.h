#pragma once

#include "CoreMinimal.h"

#include "CombatStatusEffects.generated.h"

/*
Base class to create Status Effects from.
NOTE: It is recommended to create new child classes using C++ instead for performance reasons.
*/
UCLASS(Abstract, Blueprintable, BlueprintType)
class LQTL_TT_DEVELOPMENT_API UCombatStatusEffectBase : public UObject
{
	GENERATED_BODY()

protected:
	// The current "level" (or magnitude) of this effect.
	// Each effect can define how this value affect the implication of the effect.
	UPROPERTY(BlueprintReadWrite)
	float EffectLevel;

	// The minimum level this effect could reach.
	// If the minimum level is reached, this effect will be considered "removed" from its handler.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinLevel = 0;

	// The maximum level this effect could reach.
	// Any more level applied to this effect after this value will have no value.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxLevel = 0;

	// Called when the effect is applied to a valid target.
	//
	// @return True when the effect is succesfuly applied; otherise, false.
	UFUNCTION(BlueprintNativeEvent)
	bool OnStatusEffectApplied(AActor* _Initigator, AActor* _Target, float _Level);

	// Called when the effect is removed from an owning target.
	//
	// @return True when the effect is succesfuly removed; otherise, false.
	UFUNCTION(BlueprintNativeEvent)
	bool OnStatusEffectRemoved(AActor* _Initigator, AActor* _Target, float _Level);

	virtual bool OnStatusEffectApplied_Implementation(AActor* _Initigator, AActor* _Target, float _Level);

	virtual bool OnStatusEffectRemoved_Implementation(AActor* _Initigator, AActor* _Target, float _Level);

public:
	virtual void BeginDestroy() override;

	// Apply the given level from this effect.
	// Effect behaviour can be manually defined when being applied.
	// NOTE: a negative level value should be considered as 'removing' the effect.
	bool ApplyEffect(AActor* _Initigator, AActor* _Target, float _Level);

	// Get the current level value of this effect.
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE float GetCurrentEffectLevel() { return EffectLevel; }

	// Get the minimum level value of this effect.
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE float GetMinLevel() { return MinLevel; }

	// Get the maximum level value of this effect.
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE float GetMaxLevel() { return MaxLevel; }
};

/*
Handle class for assigned effects to UAppliedEffectHandler.
*/
USTRUCT(BlueprintType)
struct FAppliedCombatEffectHandle 
{
	GENERATED_BODY()

	// The applied effect class.
	TSubclassOf<UCombatStatusEffectBase> AppliedEffectClass;

	// The actor that triggers this modification.
	AActor* Initigator;

	// The actor which recived this modification.
	AActor* Target;

	// The delta level which was effectively modifed.
	// NOTE: If = 0, no changes were applied
	float AppliedLevel;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEffectModified, FAppliedCombatEffectHandle, Handle);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEffectRemoved, TSubclassOf<UCombatStatusEffectBase>, EffectClass);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEffectAdded, TSubclassOf<UCombatStatusEffectBase>, EffectClass);

/*
Inernal class used by UAdvancedCombatSystemComponent for handling applied UCombatStatusEffects.
*/
class UAppliedEffectHandler
{
private:
	using EffectMap = TMap<TSubclassOf<UCombatStatusEffectBase>, TObjectPtr<UCombatStatusEffectBase>>;

private:
	EffectMap AppliedEffects;

public:
	// Get a list of all applied effects.
	FORCEINLINE TArray<EffectMap::KeyType> GetAppliedEffects() const { 
		TArray<EffectMap::KeyType> _OutKeys; 
		AppliedEffects.GetKeys(_OutKeys);
		return _OutKeys;
	}

	// Apply a UCombatStatusEffectBase to this handler for execution.
	// NOTE: To apply the effect, use a positive value for level; to remove the effect, use negative value instead.
	void ApplyStatusEffect(
		IN AActor* _Initigator, 
		IN AActor* _Target,
		IN TSubclassOf<UCombatStatusEffectBase> _Effect,
		IN float _Level, 
		OUT FAppliedCombatEffectHandle& _OutHandle);

	// Revert an application of effect using its handle.
	void RevertEffectApplication(const FAppliedCombatEffectHandle& _Handle);

public:
	// Fired once an effect is modified on this handler.
	// 
	// NOTE: Adding a new effect or removing an existing effect will still trigger this event alongside with OnEffectRemoved and FOnEffectAdded.
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnEffectModified OnEffectModified;

	// Fired once an existing effect is removed from the applied list.
	// An effect is considered "removed" when: Level == MinLevel
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnEffectRemoved OnEffectRemoved;

	// Fired once a new effect is added to the applied list.
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnEffectAdded OnEffectAdded;
};