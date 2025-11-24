#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BFL_Algorithm.generated.h"

/**
 * Custom implemented algorithms support the in-game requirements.
 */
UCLASS()
class LQTL_TT_DEVELOPMENT_API UBFL_Algorithm : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	/*
	* Get a random index from the input weight list, excluding indicies from {Excluded Indicies}.
	* A higher weight have a higher chance to be the result index.
	* 
	* @param _WeightList		- Input weights. The result index will match the index of elements within this list.
	* @param _ExcludedIndicies	- Indicies that will be excluded from the list during evaluation.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Algorithm|Weighted Evaluation")
	static void GetRandomWeightedIndex(
		UPARAM(meta = (DisplayName = "Weights List")) TArray<float> _WeightList,
		UPARAM(meta = (DisplayName = "Excluded Indicies")) TArray<int> _ExcludedIndicies,
		UPARAM(meta = (DisplayName = "Out Index")) int& _OutIndex);
};
