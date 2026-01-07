// Copyright Tale Weavers

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "StateTreeExecutionContext.h"
#include "LQAIGoalGenerator.generated.h"

class AActor;

/**
 *
 */
UCLASS(Abstract, EditInlineNew, DefaultToInstanced, CollapseCategories, DisplayName = "Foo Instanced")
class LQAISYSTEM_API ULQAIGoalGenerator : public UObject
{
	GENERATED_BODY()

public: 
	void StartGenerate(FStateTreeExecutionContext& Context);
	void Generate(FStateTreeExecutionContext& Context, float DeltaTime);
	void EndGenerate();

	UFUNCTION(BlueprintNativeEvent)
	bool GenerateGoal(AActor* Owner);

public: 
	UPROPERTY(EditDefaultsOnly, Category = "LQ AI : Goal Generator")
	FGameplayTag GoalTag {};

	UPROPERTY(EditDefaultsOnly, Category = "LQ AI : Goal Generator")
	bool bCheckPriority = true;
};
