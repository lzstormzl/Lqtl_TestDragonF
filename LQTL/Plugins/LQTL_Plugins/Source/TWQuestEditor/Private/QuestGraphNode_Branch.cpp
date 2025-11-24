#include "QuestGraphNode_Branch.h"

// Thư viện Plugin
#include "Quest/Quest.h"
#include "QuestEditorSettings.h"

void UQuestGraphNode_Branch::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, FName("SingleNode"), TEXT(""));
	CreatePin(EGPD_Output, FName("SingleNode"), TEXT(""));
}

void UQuestGraphNode_Branch::DestroyNode()
{
	Super::DestroyNode();

	//When the node is destroyed make sure it gets removed from the quest
	QuestNode->GetOwningQuest()->Branches.Remove(Branch);

}

FLinearColor UQuestGraphNode_Branch::GetGraphNodeColor() const
{
	return GetDefault<UQuestEditorSettings>()->TaskNodeColor;
}