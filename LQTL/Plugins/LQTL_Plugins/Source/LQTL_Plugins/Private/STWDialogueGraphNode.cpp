#include "STWDialogueGraphNode.h"

// Thư viện Unreal
#include "SGraphPin.h"
#include "Blueprint/UserWidget.h"
#include <Slate/SObjectWidget.h>
#include "Components/VerticalBox.h"

// Thư viện Dialogue Plugins
#include "Dialogue/Dialogue.h"
#include "DialogueBlueprint.h"
#include "DialogueGraphNode.h"
#include "TWDialogueEditorSettings.h"
#include "TWDialogueNodeUserWidget.h"

void STWDialogueGraphNode::Construct(const FArguments& InArgs, UDialogueGraphNode* InNode)
{
	GraphNode = InNode;

	UpdateGraphNode();

}

void STWDialogueGraphNode::CreatePinWidgets()
{
	UDialogueGraphNode* DialogueGraphNode = CastChecked<UDialogueGraphNode>(GraphNode);

	for (int32 PinIdx = 0; PinIdx < DialogueGraphNode->Pins.Num(); PinIdx++)
	{
		UEdGraphPin* MyPin = DialogueGraphNode->Pins[PinIdx];
		if (!MyPin->bHidden)
		{
			TSharedPtr<SGraphPin> NewPin = SNew(SGraphPin, MyPin);

			AddPin(NewPin.ToSharedRef());
		}
	}
}

void STWDialogueGraphNode::UpdateGraphNode()
{
    InputPins.Empty();
    OutputPins.Empty();

    RightNodeBox.Reset();
    LeftNodeBox.Reset();

	UDialogueGraphNode* DialogueGraphNode = Cast<UDialogueGraphNode>(GraphNode);
	UTWDialogueNode* DialogueNode = DialogueGraphNode ? DialogueGraphNode->TWDialogueNode : nullptr;

	UWorld* WidgetWorld = GEditor->GetEditorWorldContext().World();
	class UTWDialogueNodeUserWidget* DialogueNodeWidgetInstance = nullptr;

	if (const UTWDialogueEditorSettings* DialogueSettings = GetDefault<UTWDialogueEditorSettings>())
	{
		UClass* WidgetClass = DialogueSettings->DefaultDialogueWidgetClass.LoadSynchronous();

		// Spawn an instance of Widget
		DialogueNodeWidgetInstance = CreateWidget<UTWDialogueNodeUserWidget>(WidgetWorld, WidgetClass);
	}

	check (DialogueNodeWidgetInstance);
	//check (DialogueNode);

	if (/*DialogueNode &&*/ DialogueNodeWidgetInstance)
	{
		DialogueNodeWidgetInstance->SetFlags(RF_Transient);

		if (UDialogueBlueprint* DialogueBP = Cast<UDialogueBlueprint>(GraphNode->GetGraph()->GetOuter()))
		{
			if (UDialogue* OwningDialogue = Cast<UDialogue>(DialogueBP->GeneratedClass->GetDefaultObject()))
			{
				DialogueNodeWidgetInstance->InitializeFromNode(DialogueNode, OwningDialogue);
				DialogueNodeWidgetRef = DialogueNodeWidgetInstance->TakeWidget();

				if (DialogueNodeWidgetInstance->RightPinBox && DialogueNodeWidgetInstance->LeftPinBox)
				{
					//Tell the C++ code to autopopulate our UMG blueprint widgets pin boxes! 
					RightNodeBox = StaticCastSharedRef<SVerticalBox>(DialogueNodeWidgetInstance->RightPinBox->TakeWidget());
					LeftNodeBox = StaticCastSharedRef<SVerticalBox>(DialogueNodeWidgetInstance->LeftPinBox->TakeWidget());
				}
				//else
				//{
				//	//Didnt have pin boxes, clear the dialogue node ref to force node to use old style 
				//	//DialogueNodeWidgetRef.Reset();
				//}
			}
		}

	}

	if (DialogueNodeWidgetRef.IsValid())
	{
		this->GetOrAddSlot(ENodeZone::Center)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SOverlay)
				+ SOverlay::Slot()
			[
					DialogueNodeWidgetRef.ToSharedRef()
			]
		];
	}

    CreatePinWidgets();
}

void STWDialogueGraphNode::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
	PinToAdd->SetOwner(SharedThis(this));

	
	if (!RightNodeBox.IsValid() || !LeftNodeBox.IsValid())
	{
		return;
	}

	const UEdGraphPin* PinObj = PinToAdd->GetPinObj();

	if (PinToAdd->GetDirection() == EEdGraphPinDirection::EGPD_Input)
	{
		LeftNodeBox->AddSlot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.FillHeight(1.0f)
			.Padding(5.f, 0.f)
			[
				PinToAdd
			];
		InputPins.Add(PinToAdd);
	}
	else
	{
		RightNodeBox->AddSlot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.FillHeight(1.0f)
			[
				PinToAdd
			];
		OutputPins.Add(PinToAdd);
	}
}