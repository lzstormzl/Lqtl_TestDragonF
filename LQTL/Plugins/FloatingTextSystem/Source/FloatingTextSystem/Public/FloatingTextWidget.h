// Copyright Tale Weavers

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FloatingTextTypes.h"
#include "FloatingTextWidget.generated.h"

/**
 * Base widget class for floating text
 * Extend this in Blueprint to create custom floating text visuals
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class FLOATINGTEXTSYSTEM_API UFloatingTextWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * Initialize widget with typed data
	 * Override in Blueprint to handle specific data types
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Floating Text")
	void InitializeWithData(const TInstancedStruct<FFloatingTextDataBase>& InData);
	virtual void InitializeWithData_Implementation(const TInstancedStruct<FFloatingTextDataBase>& InData);

	/**
	 * Called when widget is activated from pool
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Floating Text")
	void OnActivated();
	virtual void OnActivated_Implementation();

	/**
	 * Called when widget is deactivated and returned to pool
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Floating Text")
	void OnDeactivated();
	virtual void OnDeactivated_Implementation();

	/**
	 * Reset widget state for reuse
	 */
	UFUNCTION(BlueprintCallable, Category = "Floating Text")
	virtual void ResetWidget();

protected:
	/** Cached data for this widget instance */
	UPROPERTY(BlueprintReadOnly, Category = "Floating Text")
	TInstancedStruct<FFloatingTextDataBase> CachedData;
};
