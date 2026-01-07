// Copyright Tale Weavers

#pragma once

#include "CoreMinimal.h"
#include "Action/ComboActionPass.h"
#include "StructUtils/InstancedStruct.h"
#include "LQComboActionPass_ActivateAbility.generated.h"

struct FLQAbilityPayloadDataBase;
class ULQGameplayAbility;
/**
 * 
 */
UCLASS(DisplayName="LQ.Activate Combo Ability")
class LQCOMBOGRAPHEXTENSIONRUNTIME_API ULQComboActionPass_ActivateAbility : public UComboActionPass
{
	GENERATED_BODY()

protected:
	virtual bool ExecutePass_Implementation(UComboGraphInstance* GraphInstance) const override;

	virtual TArray<FSoftObjectPath> GetPreviewAssets() const override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ULQGameplayAbility> AbilityClass;

	UPROPERTY(EditAnywhere, meta=(ExcludeBaseStruct))
	TInstancedStruct<FLQAbilityPayloadDataBase> Payload;

	virtual FString GetPassInformation_Implementation() const override;

#if WITH_EDITOR
	void SetAttackTypeMutipllierOnChanged();
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
