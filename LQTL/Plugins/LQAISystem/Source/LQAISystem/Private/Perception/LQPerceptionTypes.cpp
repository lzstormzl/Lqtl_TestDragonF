// Copyright Tale Weavers

#include "Perception/LQPerceptionTypes.h"
#include "Perception/LQAIPerceptionComponent.h"

// ============================================================================
// CONSOLE VARIABLES
// ============================================================================

bool bDrawDebugPerception = false;
static FAutoConsoleVariableRef CVarDrawDebugPerception(
	TEXT("LQAI.DebugPerception"),
	bDrawDebugPerception,
	TEXT("Enable perception debug visualization every tick.\n0 = Off, 1 = On"),
	ECVF_Cheat
);

// ============================================================================
// FLQAIPerceptionContext
// ============================================================================

bool FLQAIPerceptionContext::IsValid() const
{
	return PerceptionComponent.IsValid();
}

AActor* FLQAIPerceptionContext::GetOwner() const
{
	if (PerceptionComponent.IsValid())
	{
		return PerceptionComponent->GetOwner();
	}
	return nullptr;
}

UWorld* FLQAIPerceptionContext::GetWorld() const
{
	if (PerceptionComponent.IsValid())
	{
		return PerceptionComponent->GetWorld();
	}
	return nullptr;
}

// ============================================================================
// FLQAIPerceptionScore
// ============================================================================

void FLQAIPerceptionScore::RecalculateTotalWeight()
{
	TotalWeight = 0.0f;
	for (const TPair<FGameplayTag, float>& Pair : HandleWeights)
	{
		TotalWeight += Pair.Value;
	}
}

bool FLQAIPerceptionScore::operator>(const FLQAIPerceptionScore& Other) const
{
	return TotalWeight > Other.TotalWeight;
}

