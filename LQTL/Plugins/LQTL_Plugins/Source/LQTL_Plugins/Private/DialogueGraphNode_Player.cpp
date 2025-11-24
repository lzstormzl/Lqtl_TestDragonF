#include "DialogueGraphNode_Player.h"
#include "TWDialogueGraph.h"
#include "EdGraph/EdGraphSchema.h"
//#include "DialogueEditorTypes.h"
#include "TWDialogueEditorSettings.h"
#include "Dialogue/Dialogue.h"
#include "DialogueGraphNode.h" 

#define LOCTEXT_NAMESPACE "DialogueGraphNode_Player"

void UDialogueGraphNode_Player::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, "SingleNode", TEXT(""));
	CreatePin(EGPD_Output, "SingleNode", TEXT(""));
}

FLinearColor UDialogueGraphNode_Player::GetGraphNodeColor() const
{
	return GetDefault<UTWDialogueEditorSettings>()->PlayerNodeColor;
}

FText UDialogueGraphNode_Player::GetNodeTitleText() const
{
	return LOCTEXT("PlayerNodeTitleText", "Player");
}

#undef LOCTEXT_NAMESPACE
