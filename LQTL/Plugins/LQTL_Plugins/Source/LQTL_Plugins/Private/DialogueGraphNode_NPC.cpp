#include "DialogueGraphNode_NPC.h"

#include "EdGraph/EdGraphSchema.h"

void UDialogueGraphNode_NPC::AllocateDefaultPins()
{
    CreatePin(EGPD_Input, "SingleNode", "");
    CreatePin(EGPD_Output, "SingleNode", "");
}

FLinearColor UDialogueGraphNode_NPC::GetGraphNodeColor() const
{
    return FLinearColor(1.0f,1.0f,1.0f,1.0f);
}