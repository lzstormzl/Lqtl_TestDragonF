#include "QuestGraphNode_Root.h"

#include "QuestEditorSettings.h"

void UQuestGraphNode_Root::AllocateDefaultPins()
{
	CreatePin(EGPD_Output, FName("SingleNode"), TEXT(""));
}

bool UQuestGraphNode_Root::CanUserDeleteNode() const
{
	return false;
}

FLinearColor UQuestGraphNode_Root::GetGraphNodeColor() const
{
	return GetDefault<UQuestEditorSettings>()->RootNodeColor;
}
