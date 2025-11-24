#include "DialogueGraphNode.h"
#include "TWDialogueGraph.h"
#include "Dialogue/TWDialogueNode.h"

FText UDialogueGraphNode::GetNodeText() const
{
    return TWDialogueNode->GetDialogueLine().DisplayText;
}

void UDialogueGraphNode::PostPlacedNewNode()
{
	if (UTWDialogueGraph* DialogueGraph = Cast<UTWDialogueGraph>(GetGraph()))
	{
		DialogueGraph->NodeAdded(this);
	}
}

void UDialogueGraphNode::AutowireNewNode(UEdGraphPin* FromPin)
{
	Super::AutowireNewNode(FromPin);

	if (FromPin != nullptr)
	{
		UEdGraphPin* OutputPin = GetOutputPin();

		if(GetSchema()->TryCreateConnection(FromPin, GetInputPin()))
		{
			FromPin->GetOwningNode()->NodeConnectionListChanged();
		}
		else if (OutputPin != nullptr && GetSchema()->TryCreateConnection(OutputPin, FromPin))
		{
			NodeConnectionListChanged();
		}
	}	
}

void UDialogueGraphNode::PinConnectionListChanged(UEdGraphPin* Pin)
{
	if (UTWDialogueGraph* DialogueGraph = Cast<UTWDialogueGraph>(GetGraph()))
	{
		DialogueGraph->PinRewired(this, Pin);
	}
}

UEdGraphPin* UDialogueGraphNode::GetInputPin(int32 InputIndex /*= 0*/) const
{
	check(InputIndex >= 0);

	for (int32 PinIndex = 0, FoundInputs = 0; PinIndex < Pins.Num(); PinIndex++)
	{
		if (Pins[PinIndex]->Direction == EGPD_Input)
		{
			if (InputIndex == FoundInputs)
			{
				return Pins[PinIndex];
			}
			else
			{
				FoundInputs++;
			}
		}
	}

	return nullptr;
}

UEdGraphPin* UDialogueGraphNode::GetOutputPin(int32 InputIndex /*= 0*/) const
{
	check(InputIndex >= 0);

	for (int32 PinIndex = 0, FoundInputs = 0; PinIndex < Pins.Num(); PinIndex++)
	{
		if (Pins[PinIndex]->Direction == EGPD_Output)
		{
			if (InputIndex == FoundInputs)
			{
				return Pins[PinIndex];
			}
			else
			{
				FoundInputs++;
			}
		}
	}

	return nullptr;
}

void UDialogueGraphNode::OnStartedOrFinished(UTWDialogueNode* Node, bool bStarted)
{
	
}