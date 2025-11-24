#include "QuestGraphNode_PersistentTasks.h"

#include "QuestEditorSettings.h"
#include "Quest/QuestSM.h"

void UQuestGraphNode_PersistentTasks::AllocateDefaultPins()
{
	CreatePin(EGPD_Output, FName("SingleNode"), TEXT(""));
}

bool UQuestGraphNode_PersistentTasks::CanUserDeleteNode() const
{
	return false;
}

FLinearColor UQuestGraphNode_PersistentTasks::GetGraphNodeColor() const
{
	return GetDefault<UQuestEditorSettings>()->PersistentTasksNodeColor;
}
