#include "TWDialogueNodeUserWidget.h"

void UTWDialogueNodeUserWidget::InitializeFromNode(class UTWDialogueNode* InNode, class UDialogue* InDialogue)
{
	if (InNode)
	{
		Node = InNode;
		//Dialogue = InDialogue;

		//OnNodeInitialized(InNode, InDialogue);
	}
}