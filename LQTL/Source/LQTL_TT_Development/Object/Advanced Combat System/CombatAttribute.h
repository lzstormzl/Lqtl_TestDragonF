#pragma once

#include "CoreMinimal.h"
#include "GameplayTags.h"

#include "CombatAttribute.generated.h"

/*
Base class for all types of combat attributes.
NOTE: All attributes should be stored as numeric types (inner is stored value as float)
*/
UCLASS(Blueprintable, BlueprintType, Abstract, EditInlineNew)
class LQTL_TT_DEVELOPMENT_API UCombatAttribute : public UObject
{
	GENERATED_BODY()

private:
	float Value;

protected:

	// Called right before any modification is applied to the generic value of this attribute
	UFUNCTION(BlueprintNativeEvent)
	void PreModifyValue();
	
	// Called right after any modification is applied to the generic value of this attribute
	UFUNCTION(BlueprintNativeEvent)
	void PostModifyValue();

	virtual void PreModifyValue_Implementation() { }

	virtual void PostModifyValue_Implementation() { }

public:
	// Tags are used to categorize attributes into groups
	// Any tag can be used within the gameplay tags.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer AttributeTags;

	// Get the value of this attribute 
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE float GetValue() const { return Value; }

	// Set the value of this attribute
	FORCEINLINE void SetValue(float _Value) {
		PreModifyValue_Implementation();
		Value = _Value;
		PostModifyValue_Implementation();
	}
};

/*
Contains data of combat atrtibute modification
*/
USTRUCT(BlueprintType)
struct LQTL_TT_DEVELOPMENT_API FCombatAttributeModifyCallback 
{
	GENERATED_BODY();

	UPROPERTY()
	TObjectPtr<UCombatAttribute> Attribute;

	UPROPERTY()
	float OriginalValue;

	UPROPERTY()
	float ModifiedValue;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeModified, FCombatAttributeModifyCallback, CallbackData);

/*
Contains a set of UCombatAttribute used for 
*/
UCLASS(Blueprintable, BlueprintType, EditInlineNew)
class LQTL_TT_DEVELOPMENT_API UCombatAttributeSet : public UObject
{
	GENERATED_BODY()

private:
	TMap<FGameplayTag, TArray<TObjectPtr<UCombatAttribute>>> AttributeTagLookup;

	TMap<TSubclassOf<UCombatAttribute>, TObjectPtr<UCombatAttribute>> AttributeClassLookup;

	// Used for editor property updates only
	TArray<TObjectPtr<UCombatAttribute>> CachedAttributes;

	// Set value of the given attribute
	// This function handles all internal logic for modifying an attribute value
	void SetAttribute(TObjectPtr<UCombatAttribute> _Attribute, float _Value);

public:
	// Attributes assigned to this 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced)
	TArray<TObjectPtr<UCombatAttribute>> Attributes;

#if WITH_EDITOR
	void PreEditChange(FProperty* _PropertyAboutToChange) override;

	void PostEditChangeProperty(FPropertyChangedEvent& _PropertyChangedEvent) override;

	void PostLoad() override;
#endif

	// Add the given value to the given UCombatAttribute if the attribute set contains it
	//		Returns: true if the given attribute was found; otherwise, false
	UFUNCTION(BlueprintCallable, Category = "Combat System|Attribute")
	bool AddAttributeByClass(TSubclassOf<UCombatAttribute> _AttributeClass, float _Value);

	// Add the given value to all attributes containing the given tag.
	//		Returns: true if any attribute was found containing the same tag; otherwise, false
	UFUNCTION(BlueprintCallable, Category = "Combat System|Attribute")
	bool AddAttributeByTag(FGameplayTag _AttributeTag, float _Value);

	// Multiply the given value to the given UCombatAttribute if the attribute set contains it
	//		Returns: true if the given attribute was found; otherwise, false
	UFUNCTION(BlueprintCallable, Category = "Combat System|Attribute")
	bool MultAttributeByClass(TSubclassOf<UCombatAttribute> _AttributeClass, float _Value);

	// Multiply the given value to all attributes containing the given tag.
	//		Returns: true if any attribute was found containing the same tag; otherwise, false
	UFUNCTION(BlueprintCallable, Category = "Combat System|Attribute")
	bool MultAttributeByTag(FGameplayTag _AttributeTag, float _Value);

public:
	// 
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnAttributeModified OnAttributeModified;
};