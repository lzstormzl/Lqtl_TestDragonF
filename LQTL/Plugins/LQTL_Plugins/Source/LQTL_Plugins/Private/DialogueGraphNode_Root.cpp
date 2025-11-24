
#include "DialogueGraphNode_Root.h"
//#include "DialogueEditorTypes.h"
#include "TWDialogueEditorSettings.h"

void UDialogueGraphNode_Root::AllocateDefaultPins()
{
	CreatePin(EGPD_Output, "SingleNode", TEXT(""));
}

bool UDialogueGraphNode_Root::CanUserDeleteNode() const
{
	return false;
}