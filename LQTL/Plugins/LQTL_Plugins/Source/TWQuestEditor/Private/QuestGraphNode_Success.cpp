#include "QuestGraphNode_Success.h"
#include "QuestEditorSettings.h"


void UQuestGraphNode_Success::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, FName("SingleNode"), TEXT(""));
}

FLinearColor UQuestGraphNode_Success::GetGraphNodeColor() const
{
	return GetDefault<UQuestEditorSettings>()->SuccessNodeColor;
}
