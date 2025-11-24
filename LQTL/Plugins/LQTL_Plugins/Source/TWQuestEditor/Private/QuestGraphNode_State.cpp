#include "QuestGraphNode_State.h"

// Thư viện Unreal

// Thư viện Plugin
#include "QuestEditorSettings.h"
#include "Quest/QuestSM.h"
#include "Quest/Quest.h"

void UQuestGraphNode_State::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, FName("SingleNode"), TEXT(""));
	CreatePin(EGPD_Output, FName("SingleNode"), TEXT(""));
}

void UQuestGraphNode_State::DestroyNode()
{
	Super::DestroyNode();

	check(QuestNode->GetOwningQuest());

	//When the node is destroyed make sure it gets removed from the quest
	QuestNode->GetOwningQuest()->States.Remove(State);
}

FLinearColor UQuestGraphNode_State::GetGraphNodeColor() const
{
	return GetDefault<UQuestEditorSettings>()->StateNodeColor;
}