#include "BFL_Algorithm.h"

void UBFL_Algorithm::GetRandomWeightedIndex(
	TArray<float> _WeightList, 
	TArray<int> _ExcludedIndicies, 
	int& _OutIndex)
{
	TMap<int, float> mappedWeightIndex;
	float totalWeight = 0;

	for (int i = 0; i < _WeightList.Num(); i++) {
		if (_ExcludedIndicies.Contains(i)) continue;

		totalWeight += _WeightList[i];
		mappedWeightIndex.Add(i, _WeightList[i]);
	}

	// If there is no indicies left, return -1.
	if (mappedWeightIndex.IsEmpty()) {
		_OutIndex = -1;
		return;
	}

	// 1. Sort the weights from largest to smallest.
	// 2. Loop through the list and add the current weight to the evaluated weight. 
	// 3. Check if the evaluated weight is pass the random weight.
	// 
	// If true, exit the algorithm and returns the index of that weight.
	// Otherwise, continue.
	//
	mappedWeightIndex.ValueSort(
		[](float a, float b) { 
			return a > b; 
		}
	);

	float evaluatedWeight = 0, randomWeight = FMath::FRandRange(0.f, totalWeight);

	for (auto pair : mappedWeightIndex) {
		evaluatedWeight += pair.Value;

		if (evaluatedWeight >= randomWeight) {
			_OutIndex = pair.Key;
			return;
		}
	}
}