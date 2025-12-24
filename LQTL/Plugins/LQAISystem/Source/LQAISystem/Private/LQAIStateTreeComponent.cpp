// Copyright Tale Weavers


#include "LQAIStateTreeComponent.h"
#include "LQAIDefinitionDataAsset.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "StateTree.h"
#include "BehaviorTree/BehaviorTree.h"

ULQAIStateTreeComponent::ULQAIStateTreeComponent()
{

}

void ULQAIStateTreeComponent::BeginPlay()
{
	Super::BeginPlay();

	if (ensure(!AIDefinitionSoftAsset.IsNull()))
	{
		if (!AIDefinitionSoftAsset.IsValid())
		{
			UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(AIDefinitionSoftAsset.ToSoftObjectPath(), FStreamableDelegate::CreateLambda([WeakThis = MakeWeakObjectPtr(this)]() {
				if (ULQAIStateTreeComponent* StrongThis = WeakThis.Get())
				{
					StrongThis->OnDefinitionAssetLoaded();	
				}
			}));
		}
		else
		{
			OnDefinitionAssetLoaded();
		}
	}
	else
	{
		UE_LOG(LogAI, Warning, TEXT("Invalid AI definition data asset"));
	}
}

bool ULQAIStateTreeComponent::AddGoal(const FGameplayTag& NewGoal)
{
	if (!NewGoal.IsValid())
	{
		UE_LOG(LogAI, Warning, TEXT("Invalid goal tag to add"));
		return false;
	}

	if (!AIDefinitionAsset)
	{
		UE_LOG(LogAI, Warning, TEXT("Invalid AI definition asset"));
		return false;
	}

	FLQAIGoalPriorityData* GoalPriority = AIDefinitionAsset->GoalDataset.FindByPredicate([GoalToFind = NewGoal](const FLQAIGoalPriorityData& GoalData){
		return GoalData.GoalTag.MatchesTagExact(GoalToFind);
	});

	if (GoalPriority)
	{
		FLQAIGoalPriorityData LastHighestPriorityGoal;
		GetHighestPriorityGoalData(LastHighestPriorityGoal);

		OnGoingGoals.AddUnique(*GoalPriority);
		OnGoingGoals.Sort([](const FLQAIGoalPriorityData& L, const FLQAIGoalPriorityData& R){
			return L < R;
		});

		FLQAIGoalPriorityData NewHighestPriorityGoal;
		if (GetHighestPriorityGoalData(NewHighestPriorityGoal) && NewHighestPriorityGoal != LastHighestPriorityGoal)
		{
			OnHighestPriorityGoalChanged(LastHighestPriorityGoal, NewHighestPriorityGoal);
		}
		LogOnGoingGoals();
		return true;
	}
	FString OwnerDisplayName = GetOwner() ? GetOwner()->GetName() : TEXT("");
	UE_LOG(LogAI, Verbose, TEXT("(%s) has no goal (%s) defined in goal dataset"), *OwnerDisplayName, *NewGoal.ToString());
	return false;
}

bool ULQAIStateTreeComponent::K2_AddGoal(FGameplayTag NewGoal)
{
	return AddGoal(NewGoal);
}

bool ULQAIStateTreeComponent::RemoveGoal(const FGameplayTag& Goal)
{
	if (!Goal.IsValid())
	{
		UE_LOG(LogAI, Warning, TEXT("Invalid goal tag to remove"));
		return false;
	}

	int32 GoalIndex = OnGoingGoals.IndexOfByPredicate([GoalToFind = Goal](const FLQAIGoalPriorityData& GoalData) {
		return GoalData.GoalTag.MatchesTagExact(GoalToFind);
	});

	if (GoalIndex >= 0)
	{
		FLQAIGoalPriorityData LastHighestGoal;
		GetHighestPriorityGoalData(LastHighestGoal);

		OnGoingGoals.RemoveAt(GoalIndex);
		OnGoingGoals.Sort([](const FLQAIGoalPriorityData& L, const FLQAIGoalPriorityData& R) {
			return L < R;
		});

		FLQAIGoalPriorityData NewHighestGoal;
		if (GetHighestPriorityGoalData(NewHighestGoal))
		{
			if (LastHighestGoal != NewHighestGoal)
			{
				OnHighestPriorityGoalChanged(LastHighestGoal, NewHighestGoal);
			}
		}
		LogOnGoingGoals();
		return true;
	}
	FString OwnerDisplayName = GetOwner() ? GetOwner()->GetName() : TEXT("");
	UE_LOG(LogAI, Verbose, TEXT("(%s) has no on going goal (%s) to remove"), *OwnerDisplayName, *Goal.ToString());
	return false;
}

bool ULQAIStateTreeComponent::K2_RemoveGoal(FGameplayTag Tag)
{
	return RemoveGoal(Tag);
}

void ULQAIStateTreeComponent::OnHighestPriorityGoalChanged(const FLQAIGoalPriorityData& OldGoal, const FLQAIGoalPriorityData& NewGoal)
{

}

UStateTree* ULQAIStateTreeComponent::GetNonCombatCoreStateTree() const
{
	return LoadedAIData.NonCombatCoreStateTree;
}

UStateTree* ULQAIStateTreeComponent::GetCombatCoreStateTree() const
{
	return LoadedAIData.CombatCoreStateTree;
}

UStateTree* ULQAIStateTreeComponent::GetAdditionalStateTreeByTag(const FGameplayTag& Tag) const
{
	if (const TObjectPtr<UStateTree>* FoundStateTree = LoadedAIData.AdditionalStateTreeMap.Find(Tag))
	{
		return *FoundStateTree;
	}
	return nullptr;
}

UBehaviorTree* ULQAIStateTreeComponent::GetBehaviorTreeByTag(const FGameplayTag& Tag) const
{
	if (const TObjectPtr<UBehaviorTree>* FoundBT = LoadedAIData.BehaviorTreeMap.Find(Tag))
	{
		return *FoundBT;
	}
	return nullptr;
}

bool ULQAIStateTreeComponent::GetHighestPriorityGoalData(FLQAIGoalPriorityData& OutGoalData, bool bSort /*= false*/)
{
	if (bSort)
	{
		OnGoingGoals.Sort([](const FLQAIGoalPriorityData& L, const FLQAIGoalPriorityData& R) {
			return L < R;
		});
	}
	int32 HighestPriorityIndex = OnGoingGoals.Num() - 1;
	if (OnGoingGoals.IsValidIndex(HighestPriorityIndex))
	{
		OutGoalData = OnGoingGoals[HighestPriorityIndex];
		return true;
	}
	return false;
}

void ULQAIStateTreeComponent::OnDefinitionAssetLoaded()
{
	AIDefinitionAsset = AIDefinitionSoftAsset.Get();

	if (!AIDefinitionAsset)
	{
		UE_LOG(LogAI, Warning, TEXT("Failed to load AI definition asset"));
		return;
	}

	TArray<FSoftObjectPath> AssetsToLoad;
	if (!AIDefinitionAsset->NonCombatCoreStateTree.IsNull())
	{
		AssetsToLoad.Add(AIDefinitionAsset->NonCombatCoreStateTree.ToSoftObjectPath());
	}

	if (!AIDefinitionAsset->CombatCoreStateTree.IsNull())
	{
		AssetsToLoad.Add(AIDefinitionAsset->CombatCoreStateTree.ToSoftObjectPath());
	}
	for (const auto& Pair : AIDefinitionAsset->AdditionalTagToStateTreeMap)
	{
		if (!Pair.Value.IsNull())
		{
			AssetsToLoad.Add(Pair.Value.ToSoftObjectPath());
		}
	}
	for (const auto& Pair : AIDefinitionAsset->TagToBTMap)
	{
		if (!Pair.Value.IsNull())
		{
			AssetsToLoad.Add(Pair.Value.ToSoftObjectPath());
		}
	}

	if (AssetsToLoad.Num() > 0)
	{
		UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
			AssetsToLoad,
			FStreamableDelegate::CreateLambda([WeakThis = MakeWeakObjectPtr(this)]()
			{
				if (ULQAIStateTreeComponent* StrongThis = WeakThis.Get())
				{
					StrongThis->OnAIAssetsLoaded();
				}
			})
		);
	}
	else
	{
		OnAIAssetsLoaded();
	}
}

void ULQAIStateTreeComponent::OnAIAssetsLoaded()
{
	if (!AIDefinitionAsset)
	{
		return;
	}

	LoadedAIData.NonCombatCoreStateTree = AIDefinitionAsset->NonCombatCoreStateTree.Get();
	LoadedAIData.CombatCoreStateTree = AIDefinitionAsset->CombatCoreStateTree.Get();

	LoadedAIData.AdditionalStateTreeMap.Empty();
	for (const auto& Pair : AIDefinitionAsset->AdditionalTagToStateTreeMap)
	{
		if (UStateTree* LoadedStateTree = Pair.Value.Get())
		{
			LoadedAIData.AdditionalStateTreeMap.Add(Pair.Key, LoadedStateTree);
		}
	}

	LoadedAIData.BehaviorTreeMap.Empty();
	for (const auto& Pair : AIDefinitionAsset->TagToBTMap)
	{
		if (UBehaviorTree* LoadedBT = Pair.Value.Get())
		{
			LoadedAIData.BehaviorTreeMap.Add(Pair.Key, LoadedBT);
		}
	}

	// Initialize default goals
	if (AIDefinitionAsset->DefaultGoals.Num() > 0)
	{
		for (const FGameplayTag& DefaultGoal : AIDefinitionAsset->DefaultGoals)
		{
			AddGoal(DefaultGoal);
		}
	}

	UE_LOG(LogAI, Log, TEXT("AI assets loaded successfully for %s"), *GetOwner()->GetName());
}

void ULQAIStateTreeComponent::LogOnGoingGoals()
{
	UE_LOG(LogAI, Verbose, TEXT("Ongoing Goals:"));
	for (int32 Iter = OnGoingGoals.Num() - 1; Iter >= 0; Iter--)
	{
		if (OnGoingGoals[Iter].GoalTag.IsValid())
		{
			UE_LOG(LogAI, Verbose, TEXT("	%s, %d"), *OnGoingGoals[Iter].GoalTag.ToString(), OnGoingGoals[Iter].Priority);
		}
	}
}
