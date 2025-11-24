#include "TWDialogueGraphSchema.h"

//Thư viện của Dialogue plugins
#include "Dialogue/TWDialogueNode.h"
#include "DialogueGraphNode_NPC.h"
#include "DialogueGraphNode_Player.h"
#include "DialogueGraphNode_Root.h"
#include "TWDialogueGraph.h"
#include "DialogueBlueprint.h"
#include "DialogueGraphEditor.h"


// Dùng Default của Engine
#include "ScopedTransaction.h"
#include "EdGraphNode_Comment.h"

#define LOCTEXT_NAMESPACE "DialogueGraphSchema"

static int32 NodeDistance = 60;
 
UEdGraphNode* FDialogueSchemaAction_NewNode::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	UEdGraphNode* ResultNode = NULL;

	// If there is a template, we actually use it
	if (NodeTemplate != NULL)
	{
		const FScopedTransaction Transaction(FText::FromString("Add Node"));
		ParentGraph->Modify();
		if (FromPin)
		{
			FromPin->Modify();
		}

		NodeTemplate->SetFlags(RF_Transactional);

		// set outer to be the graph so it doesn't go away
		NodeTemplate->Rename(NULL, ParentGraph, REN_NonTransactional);
		ParentGraph->AddNode(NodeTemplate, true);

		NodeTemplate->CreateNewGuid();
		NodeTemplate->PostPlacedNewNode();

		// For input pins, new node will generally overlap node being dragged off
		// Work out if we want to visually push away from connected node
		int32 XLocation = Location.X;
		if (FromPin && FromPin->Direction == EGPD_Input)
		{
			UEdGraphNode* PinNode = FromPin->GetOwningNode();
			const float XDelta = FMath::Abs(PinNode->NodePosX - Location.X);

			if (XDelta < NodeDistance)
			{
				// Set location to edge of current node minus the max move distance
				// to force node to push off from connect node enough to give selection handle
				XLocation = PinNode->NodePosX - NodeDistance;
			}
		}

		NodeTemplate->NodePosX = XLocation;
		NodeTemplate->NodePosY = Location.Y;
		NodeTemplate->SnapToGrid(16);

		// setup pins after placing node in correct spot, since pin sorting will happen as soon as link connection change occurs
		NodeTemplate->AllocateDefaultPins();
		NodeTemplate->AutowireNewNode(FromPin);

		if (UDialogueGraphNode_NPC* NPCGraphNode = Cast<UDialogueGraphNode_NPC>(NodeTemplate))
		{
			//if (UDialogueNode_NPC* NPCNode = Cast<UDialogueNode_NPC>(NPCGraphNode->DialogueNode))
			//{
				//NPCNode->SpeakerID = SpeakerInfo.SpeakerID;
			//}
		}

		ResultNode = NodeTemplate;
	}

	return ResultNode;
}

UEdGraphNode* FDialogueSchemaAction_NewNode::PerformAction(class UEdGraph* ParentGraph, TArray<UEdGraphPin*>& FromPins, const FVector2D Location, bool bSelectNewNode)
{
	UEdGraphNode* ResultNode = NULL;
	if (FromPins.Num() > 0)
	{
		ResultNode = PerformAction(ParentGraph, FromPins[0], Location);

		// Try autowiring the rest of the pins
		for (int32 Index = 1; Index < FromPins.Num(); ++Index)
		{
			ResultNode->AutowireNewNode(FromPins[Index]);
		}
	}
	else
	{
		ResultNode = PerformAction(ParentGraph, NULL, Location, bSelectNewNode);
	}

	return ResultNode;
}

void FDialogueSchemaAction_NewNode::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEdGraphSchemaAction::AddReferencedObjects(Collector);

	// These don't get saved to disk, but we want to make sure the objects don't get GC'd while the action array is around
	Collector.AddReferencedObject(NodeTemplate);
}

void UTWDialogueGraphSchema::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
    if (UTWDialogueGraph* DialogueGraph = CastChecked<UTWDialogueGraph>(&Graph))
    {
        FGraphNodeCreator<UDialogueGraphNode_Root> RootNodeCreator(Graph);
        UDialogueGraphNode_Root* NewNode = RootNodeCreator.CreateNode();
		//NewNode->AllocateDefaultPins();
        RootNodeCreator.Finalize();

        // Set node này là DefaultGraphNode để Editor nhận diện được để làm gì đó
        SetNodeMetaData(NewNode, FNodeMetadata::DefaultGraphNode);


    }
}

const FPinConnectionResponse UTWDialogueGraphSchema::CanCreateConnection(const UEdGraphPin* NodeA, const UEdGraphPin* NodeB) const
{
    if (!NodeA || !NodeB)
    {
        return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Node đang null"));
    }

    bool bPinAIsNPC = NodeA->GetOwningNode()->IsA<UDialogueGraphNode_NPC>();
	bool bPinBIsNPC = NodeB->GetOwningNode()->IsA<UDialogueGraphNode_NPC>();

	if (NodeA == NodeB)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinErrorSelfDisallow", "Không thể tự nối vào bản thân nó."));
	}

	if (!bPinAIsNPC && !bPinBIsNPC)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinErrorPlayerDisallow", "Không thể kết player choice tới 1 player choice khác."));
	}

	if (NodeA->Direction != NodeB->Direction)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, LOCTEXT("PinConnect", "Nối node"));
	}
	else
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("InputsMatching", "Không thể kết nối 1 đầu ra với 1 đầu ra và 1 đầu vào với đầu vào."));
	}
}

void UTWDialogueGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{

	const bool bNoParent = (ContextMenuBuilder.FromPin == NULL);
	const bool bIsNPC = ContextMenuBuilder.FromPin ? ContextMenuBuilder.FromPin->GetOwningNode()->IsA<UDialogueGraphNode_NPC>() : false;

    FCategorizedGraphActionListBuilder ActionBuilder(TEXT("Danh sách node"));

    if (ContextMenuBuilder.CurrentGraph)
	{
		//Add an option for NPC speaker
		if (UDialogueBlueprint* const DialogueBlueprint = Cast<UDialogueBlueprint>(ContextMenuBuilder.CurrentGraph->GetOuter()))
		{
			if (DialogueBlueprint->GeneratedClass)
			{
				if (UDialogue* DialogueCDO = Cast<UDialogue>(DialogueBlueprint->GeneratedClass->GetDefaultObject()))
				{
					//for (auto& Speaker : DialogueCDO->Speakers)
					//{
						//FString OptionString = FString::Printf(TEXT("Add dialogue line for %s."), *Speaker.SpeakerID.ToString());
						FString OptionString = FString::Printf(TEXT("Thêm Node Dialogue mới cho NPC."));

						//Tạo ra 1 Action mới
						TSharedPtr<FDialogueSchemaAction_NewNode> AddNPC = UTWDialogueGraphSchema::AddNewNodeAction(ActionBuilder, FText::GetEmpty(), FText::FromString(OptionString), FText::GetEmpty());

						// Tạo 1 Node Graph Object, tại sao outer lại là ContextMenuBuilder.OwnerOfTemporaries thì tôi chưa biết :))
						UDialogueGraphNode* NodeNPC = NewObject<UDialogueGraphNode>(ContextMenuBuilder.OwnerOfTemporaries, UDialogueGraphNode_NPC::StaticClass());
						AddNPC->NodeTemplate = NodeNPC;
						//AddNPC->SpeakerInfo = Speaker;

						// Append Action này vào MenuContext
						ContextMenuBuilder.Append(ActionBuilder);
					//}
				}
			}
		}

		//Player responses can only be after an NPC response
		if (bIsNPC)
		{
			FString OptionString = FString::Printf(TEXT("Thêm lựa chọn cho người chơi."));

			TSharedPtr<FDialogueSchemaAction_NewNode> AddPlayer = UTWDialogueGraphSchema::AddNewNodeAction(ActionBuilder, FText::GetEmpty(), FText::FromString(OptionString), FText::GetEmpty());
			UDialogueGraphNode* NodeP = NewObject<UDialogueGraphNode>(ContextMenuBuilder.OwnerOfTemporaries, UDialogueGraphNode_Player::StaticClass());
			AddPlayer->NodeTemplate = NodeP;

			ContextMenuBuilder.Append(ActionBuilder);
		}

		if (bNoParent)
		{
			FCategorizedGraphActionListBuilder CommentActionBuilder(TEXT("Comment"));
			TSharedPtr<FDialogueSchemaAction_AddComment> AddComment = UTWDialogueGraphSchema::AddCommentAction(ContextMenuBuilder, FText::FromString("Comment"), FText::GetEmpty());
			ContextMenuBuilder.Append(CommentActionBuilder);
		}
	}

    UEdGraphSchema::GetGraphContextActions(ContextMenuBuilder);
}

TSharedPtr<FDialogueSchemaAction_NewNode> UTWDialogueGraphSchema::AddNewNodeAction(FGraphActionListBuilderBase& ContextMenuBuilder, const FText& Category, const FText& MenuDesc, const FText& Tooltip)
{
	TSharedPtr<FDialogueSchemaAction_NewNode> NewAction = TSharedPtr<FDialogueSchemaAction_NewNode>(new FDialogueSchemaAction_NewNode(Category, MenuDesc, Tooltip, 0));
	ContextMenuBuilder.AddAction(NewAction);

	return NewAction;
}

TSharedPtr<FDialogueSchemaAction_AddComment> UTWDialogueGraphSchema::AddCommentAction(FGraphActionListBuilderBase& ContextMenuBuilder, const FText& MenuDesc, const FText& Tooltip)
{
	TSharedPtr<FDialogueSchemaAction_AddComment> NewComment = TSharedPtr<FDialogueSchemaAction_AddComment>(new FDialogueSchemaAction_AddComment(MenuDesc, Tooltip));
	ContextMenuBuilder.AddAction(NewComment);
	return NewComment;
}

UEdGraphNode* FDialogueSchemaAction_AddComment::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode /*= true*/)
{
	UEdGraphNode_Comment* const CommentTemplate = NewObject<UEdGraphNode_Comment>();

	FVector2D SpawnLocation = Location;

	FDialogueGraphEditor* DialogueEditor = nullptr;
	if (UDialogueBlueprint* const DialogueBlueprint = Cast<UDialogueBlueprint>(ParentGraph->GetOuter()))
	{
		if (UAssetEditorSubsystem* AESubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>())
		{
			DialogueEditor = static_cast<FDialogueGraphEditor*>(AESubsystem->FindEditorForAsset(DialogueBlueprint, false));
		}
	}

	FSlateRect Bounds;
	if (DialogueEditor && DialogueEditor->Dialogue_GetBoundsForSelectedNodes(Bounds, 50.0f))
	{
		CommentTemplate->SetBounds(Bounds);
		SpawnLocation.X = CommentTemplate->NodePosX;
		SpawnLocation.Y = CommentTemplate->NodePosY;
	}

	UEdGraphNode* const NewNode = FEdGraphSchemaAction_NewNode::SpawnNodeFromTemplate<UEdGraphNode_Comment>(ParentGraph, CommentTemplate, SpawnLocation, bSelectNewNode);

	return NewNode;
}
