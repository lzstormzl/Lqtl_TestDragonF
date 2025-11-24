
#include "AssetTypeActions_QuestTask.h"
#include <Factories/BlueprintFactory.h>
// #include "NarrativeDataTask.h"
#include "SocialCondition.h"
#include "NarrativeEvent.h"
// #include "QuestTask.h"
#include "QuestTaskBlueprint.h"

FAssetTypeActions_QuestTask::FAssetTypeActions_QuestTask(uint32 InAssetCategory) : Category(InAssetCategory)
{

}

UClass* FAssetTypeActions_QuestTask::GetSupportedClass() const
{
	return UQuestTaskBlueprint::StaticClass();
}

uint32 FAssetTypeActions_QuestTask::GetCategories()
{
	return Category;
}

FAssetTypeActions_SocialCondition::FAssetTypeActions_SocialCondition(uint32 InAssetCategory) : Category(InAssetCategory)
{

}

UClass* FAssetTypeActions_SocialCondition::GetSupportedClass() const
{
	return USocialCondition::StaticClass();
}

uint32 FAssetTypeActions_SocialCondition::GetCategories()
{
	return Category;
}

UFactory* FAssetTypeActions_SocialCondition::GetFactoryForBlueprintType(UBlueprint* InBlueprint) const
{
	UBlueprintFactory* BlueprintFactory = NewObject<UBlueprintFactory>();
	BlueprintFactory->ParentClass = USocialCondition::StaticClass();
	return BlueprintFactory;
}

FAssetTypeActions_NarrativeEvent::FAssetTypeActions_NarrativeEvent(uint32 InAssetCategory) : Category(InAssetCategory)
{

}

UClass* FAssetTypeActions_NarrativeEvent::GetSupportedClass() const
{
	return UNarrativeEvent::StaticClass();
}

uint32 FAssetTypeActions_NarrativeEvent::GetCategories()
{
	return Category;
}

UFactory* FAssetTypeActions_NarrativeEvent::GetFactoryForBlueprintType(UBlueprint* InBlueprint) const
{
	UBlueprintFactory* BlueprintFactory = NewObject<UBlueprintFactory>();
	BlueprintFactory->ParentClass = UNarrativeEvent::StaticClass();
	return BlueprintFactory;
}
