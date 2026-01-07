// Copyright Tale Weavers

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FloatingTextTypes.h"
#include "Components/WidgetComponent.h"
#include "FloatingTextManager.generated.h"

/**
 * Component managing floating text widget pools
 * Add to actors that will spawn floating text (e.g., player controller, game mode)
 */
UCLASS(ClassGroup=(LQ), meta=(BlueprintSpawnableComponent))
class FLOATINGTEXTSYSTEM_API UFloatingTextManager : public UWidgetComponent
{
	GENERATED_BODY()

public:
	UFloatingTextManager();

	/**
	 * Get manager from actor (convenience function)
	 */
	UFUNCTION(BlueprintCallable, Category = "Floating Text")
	static UFloatingTextManager* GetFromActor(AActor* Actor);

	/**
	 * Spawn floating text
	 */
	UFUNCTION(BlueprintCallable, Category = "Floating Text")
	void SpawnFloatingText(const TInstancedStruct<FFloatingTextDataBase>& TextData);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	/** Pool storage per widget class */
	TMap<TSubclassOf<UFloatingTextWidget>, TArray<FFloatingTextWidgetInstance>> WidgetPools;

	/** Track active widget count per class */
	TMap<TSubclassOf<UFloatingTextWidget>, int32> ActiveWidgetCounts;

	/**
	 * Resolve widget class from data struct type
	 */
	TSubclassOf<UFloatingTextWidget> ResolveWidgetClass(const FFloatingTextDataBase& TextData);

	/**
	 * Acquire widget from pool (or create new if pool exhausted)
	 */
	FFloatingTextWidgetInstance* AcquireWidget(TSubclassOf<UFloatingTextWidget> FloatingWidgetClass);

	/**
	 * Release widget back to pool
	 */
	void ReleaseWidget(FFloatingTextWidgetInstance* WidgetInstance, TSubclassOf<UFloatingTextWidget> FloatingTextWidgetClass);

	/**
	 * Create new widget instance
	 */
	FFloatingTextWidgetInstance CreateWidgetInstance(TSubclassOf<UFloatingTextWidget> FloatingWidgetClass);

	/**
	 * Pre-warm widget pools
	 */
	void InitializePools();

	/**
	 * Timer callback for widget deactivation
	 */
	void OnWidgetDeactivationTimer(FFloatingTextWidgetInstance* WidgetInstance, TSubclassOf<UFloatingTextWidget> FloatingTextWidgetClass);
};
