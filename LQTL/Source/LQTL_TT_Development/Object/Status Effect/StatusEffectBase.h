#pragma once

#include "CoreMinimal.h"
#include "StatusEffectBase.generated.h"

/*
Base class to create Status Effects from.
NOTE: It is recommended to create new child classes using C++ instead for performance reasons.
*/
UCLASS(Abstract, Blueprintable, BlueprintType)
class LQTL_TT_DEVELOPMENT_API UStatusEffectBase : public UObject
{
	GENERATED_BODY()

public:
	// Called when the effect is applied to a valid target.
	UFUNCTION(BlueprintNativeEvent)
	void OnStatusEffectApplied(AActor* _Initigator, float _Level) const;

	// Called when the effect is removed from an owning target.
	UFUNCTION(BlueprintNativeEvent)
	void OnStatusEffectRemoved(AActor* _Initigator, float _Level) const;



	virtual void OnStatusEffectApplied_Implementation(AActor* _Initigator, float _Level) const;

	virtual void OnStatusEffectRemoved_Implementation(AActor* _Initigator, float _Level) const;
};