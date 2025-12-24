// Copyright Tale Weavers

#pragma once

#include "CoreMinimal.h"
#include "ComboManager/ComboManagerComponent.h"
#include "LQComboManager.generated.h"

class UComboGraphAsset;

UCLASS(ClassGroup=(Custom), Blueprintable)
class LQCOMBOGRAPHEXTENSIONRUNTIME_API ULQComboManager : public UComboManagerComponent
{
	GENERATED_BODY()

public:
	ULQComboManager();

	// Grants and activates a combo graph from an asset. Returns true if successful.
	UFUNCTION(BlueprintCallable, Category = "LQ Combo Manager")
	bool ActivateComboGraphFromAsset(const TSoftObjectPtr<UComboGraphAsset>& ComboGraphAsset);

protected:
private:
};
