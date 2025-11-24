#include "QuestGraphNode_Failure.h"
#include "QuestEditorSettings.h"


void UQuestGraphNode_Failure::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, FName("SingleNode"), TEXT(""));
}

FLinearColor UQuestGraphNode_Failure::GetGraphNodeColor() const
{
	return GetDefault<UQuestEditorSettings>()->FailedNodeColor;
}
