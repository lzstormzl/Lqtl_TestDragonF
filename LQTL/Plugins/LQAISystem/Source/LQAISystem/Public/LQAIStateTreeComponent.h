// Copyright Tale Weavers

#pragma once

#include "CoreMinimal.h"
#include "Components/StateTreeComponent.h"
#include "StateTree.h"
#include "GameplayTagContainer.h"
#include "LQAITypes.h"
#include "LQAIStateTreeComponent.generated.h"

class ULQAIDefinitionDataAsset;
class UDataAsset;

/**
 * Extended State Tree component for the LQAI system.
 * Handles automatic loading and initialization of State Tree assets at runtime.
 */
UCLASS()
class LQAISYSTEM_API ULQAIStateTreeComponent : public UStateTreeComponent 
{
	GENERATED_BODY()
	
public:
	ULQAIStateTreeComponent();

protected:
	virtual void BeginPlay() override;

public:
	bool AddGoal(const FGameplayTag& NewGoal);
	bool RemoveGoal(const FGameplayTag& Goal);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Add Goal", Categories = "LQ.AI.Goal"))
	bool K2_AddGoal(FGameplayTag NewGoal);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Remove Goal", Categories = "LQ.AI.Goal"))
	bool K2_RemoveGoal(FGameplayTag Tag);

	UFUNCTION(BlueprintPure)
	bool GetHighestPriorityGoalData(FLQAIGoalPriorityData& OutGoalData, bool bSort = false);

	void OnHighestPriorityGoalChanged(const FLQAIGoalPriorityData& OldGoal, const FLQAIGoalPriorityData& NewGoal);

	UStateTree* GetNonCombatCoreStateTree() const;
	UStateTree* GetCombatCoreStateTree() const;
	UStateTree* GetAdditionalStateTreeByTag(const FGameplayTag& Tag) const;

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Behavior Tree By Tag"))
	UBehaviorTree* GetBehaviorTreeByTag(const FGameplayTag& Tag) const;

protected:
	/** Called when the AI definition data asset has been loaded */
	void OnDefinitionAssetLoaded();

	/** Called when all AI assets (State Trees and Behavior Trees) have been loaded and cached */
	void OnAIAssetsLoaded();

	void LogOnGoingGoals();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LQ AI")
	TSoftObjectPtr<ULQAIDefinitionDataAsset> AIDefinitionSoftAsset = nullptr;

	UPROPERTY(Transient, meta = (Categories = "LQ.AI.Goals"))
	TArray<FLQAIGoalPriorityData> OnGoingGoals {};

	UPROPERTY(Transient)
	TObjectPtr<ULQAIDefinitionDataAsset> AIDefinitionAsset = nullptr;

	/** Cached loaded AI assets from the definition data asset */
	UPROPERTY(Transient)
	FLQAILoadedData LoadedAIData;
};
