#include "QuestGraphSchema.h"

// Thư viện Unreal
#include "EdGraphNode_Comment.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "AssetRegistry/AssetRegistryModule.h"

// Thư viện Plugin
#include "QuestGraphEditor.h"
#include "QuestBlueprint.h"
#include "QuestGraph.h"
#include "QuestGraphNode_Root.h"
#include "QuestGraphNode_PersistentTasks.h"
#include "QuestGraphNode_Branch.h"
#include "QuestGraphNode_Success.h"
#include "QuestGraphNode_Failure.h"
#include "Quest/QuestTask.h"

#define LOCTEXT_NAMESPACE "QuestGraphSchema"

const FName UQuestGraphSchema::GN_QuestGraph(FName(TEXT("QuestGraph")));

static int32 NodeDistance = 60;

UEdGraphNode* FQuestSchemaAction_NewNode::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	UEdGraphNode* ResultNode = NULL;

	// If there is a template, we actually use it
	if (NodeTemplate != NULL)
	{
		const FScopedTransaction Transaction(LOCTEXT("AddNode", "Add Node"));
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

		//For actions, auto-spawn a state after it 
		if (UQuestGraphNode_Branch* QuestBranch = Cast<UQuestGraphNode_Branch>(NodeTemplate))
		{
			FGraphNodeCreator<UQuestGraphNode_State> StateNodeCreator(*ParentGraph);
			UQuestGraphNode_State* StateNode = StateNodeCreator.CreateNode();
			StateNodeCreator.Finalize();

			StateNode->NodePosX = NodeTemplate->NodePosX + 500.f;
			StateNode->NodePosY = Location.Y;
			StateNode->SnapToGrid(16);

			// setup pins after placing node in correct spot, since pin sorting will happen as soon as link connection change occurs
			StateNode->AutowireNewNode(QuestBranch->GetOutputPin());
		}

		ResultNode = NodeTemplate;
	}

	return ResultNode;
}

UEdGraphNode* FQuestSchemaAction_NewNode::PerformAction(class UEdGraph* ParentGraph, TArray<UEdGraphPin*>& FromPins, const FVector2D Location, bool bSelectNewNode)
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

void FQuestSchemaAction_NewNode::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEdGraphSchemaAction::AddReferencedObjects(Collector);

	// These don't get saved to disk, but we want to make sure the objects don't get GC'd while the action array is around
	Collector.AddReferencedObject(NodeTemplate);
}

void UQuestGraphSchema::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
	if (UQuestGraph* QuestGraph = CastChecked<UQuestGraph>(&Graph))
	{

		FGraphNodeCreator<UQuestGraphNode_Root> RootNodeCreator(Graph);
		UQuestGraphNode_Root* RootNode = RootNodeCreator.CreateNode();
		RootNodeCreator.Finalize();
		SetNodeMetaData(RootNode, FNodeMetadata::DefaultGraphNode);

		// FGraphNodeCreator<UQuestGraphNode_PersistentTasks> PTNodeCreator(Graph);
		// UQuestGraphNode_PersistentTasks* PersistentTasksNode = PTNodeCreator.CreateNode();
		// PersistentTasksNode->NodePosY += 200.f;
		// QuestGraph->PTNode = PersistentTasksNode;

		// PTNodeCreator.Finalize();
		// SetNodeMetaData(PersistentTasksNode, FNodeMetadata::DefaultGraphNode);
	}
}

const FPinConnectionResponse UQuestGraphSchema::CanCreateConnection(const UEdGraphPin* NodeA, const UEdGraphPin* NodeB) const
{

	bool bPinAIsStateOrPT = NodeA->GetOwningNode()->IsA<UQuestGraphNode_State>() || NodeA->GetOwningNode()->IsA<UQuestGraphNode_PersistentTasks>();
	bool bPinBIsStateOrPT = NodeB->GetOwningNode()->IsA<UQuestGraphNode_State>() || NodeB->GetOwningNode()->IsA<UQuestGraphNode_PersistentTasks>();

	if (bPinAIsStateOrPT && !bPinBIsStateOrPT)
	{
		//Allow the state to connect as long as it is not connecting a task directly connected to the Persistent Tasks
		for (auto& PinBLinkedNode : NodeB->LinkedTo)
		{
			if (PinBLinkedNode->GetOwningNode()->IsA<UQuestGraphNode_PersistentTasks>())
			{
				return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinErrorPTDisallow", "Cannot connect state to Persistent Task."));
			}
		}

		//States can have as many nodes leading to and from them as required
		return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, LOCTEXT("PinConnect", "Connect nodes"));
	}
	else if (!bPinAIsStateOrPT && bPinBIsStateOrPT)
	{
		//Action output nodes can only go to one place
		if (NodeA->Direction == EEdGraphPinDirection::EGPD_Input)
		{
			//If the action node is connected to the persistent tasks, don't allow it to connect to a state
			for (auto& PinALinkedNode : NodeA->LinkedTo)
			{
				if (PinALinkedNode->GetOwningNode()->IsA<UQuestGraphNode_PersistentTasks>())
				{
					return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinErrorPTDisallow", "Cannot connect state to Persistent Task."));
				}
			}
			return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, LOCTEXT("PinConnect", "Connect nodes"));
		}
		else //Action input nodes can have many inputs
		{
			return FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_A, LOCTEXT("PinConnectReplace", "Replace connection"));
		}
	}

	return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinErrorNotCompatible", "Connection not compatible"));
}

void UQuestGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{

	const FName PinCategory = ContextMenuBuilder.FromPin ?
		ContextMenuBuilder.FromPin->PinType.PinCategory :
		FName("SingleNode");

	const bool bNoParent = (ContextMenuBuilder.FromPin == NULL);
	const bool bIsState = ContextMenuBuilder.FromPin ? ContextMenuBuilder.FromPin->GetOwningNode()->IsA<UQuestGraphNode_State>() : false;
	const bool bIsPT = ContextMenuBuilder.FromPin ? ContextMenuBuilder.FromPin->GetOwningNode()->IsA<UQuestGraphNode_PersistentTasks>() : false;

	if ((bIsState || bIsPT) || bNoParent)
	{

		//Todo optimize by caching these
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		FCategorizedGraphActionListBuilder ActionMainCategory("Now the player needs to...");

		if (UQuestBlueprint* const QuestAsset = Cast<UQuestBlueprint>(ContextMenuBuilder.CurrentGraph->GetOuter()))
		{
			TArray<UClass*> Subclasses;
			GetDerivedClasses(UQuestTask::StaticClass(), Subclasses);
		
			FCategorizedGraphActionListBuilder Title("NEXT THE PLAYER HAS TO:");
			ActionMainCategory.Append(Title);

			for (auto& QuestTaskClass : Subclasses)
			{
				if (!(QuestTaskClass->GetName().Contains("SKEL", ESearchCase::Type::CaseSensitive) || 
				QuestTaskClass->GetName().Contains("REINST", ESearchCase::Type::CaseSensitive) ||
				QuestTaskClass->GetName().Contains("DEADCLASS", ESearchCase::Type::CaseSensitive)))
				{
					const UQuestTask* Task = QuestTaskClass->GetDefaultObject<UQuestTask>();

					FString TaskName = QuestTaskClass->GetName();
					FString TaskCategory = "Uncategorized";
					FString TaskDescription = "This task doesn't have a description. To add one, open the Task, click 'Class Settings', and fill out the 'Blueprint Description'. Also add a display name and category!";

					if (UBlueprint* BPClass = UBlueprint::GetBlueprintFromClass(QuestTaskClass))
					{
						if (!BPClass->BlueprintDisplayName.IsEmpty())
						{
							TaskName = BPClass->BlueprintDisplayName;
						}
						if (!BPClass->BlueprintCategory.IsEmpty())
						{
							TaskCategory = BPClass->BlueprintCategory;
						}
						if (!BPClass->BlueprintDescription.IsEmpty())
						{
							TaskDescription = BPClass->BlueprintDescription;
						}
					}

					FCategorizedGraphActionListBuilder ActionCategoryBuilder("Tasks: " + TaskCategory);

					TSharedPtr<FQuestSchemaAction_NewNode> AddAction = UQuestGraphSchema::AddNewNodeAction(ActionCategoryBuilder, FText::GetEmpty(), FText::FromString(TaskDescription), FText::FromString(TaskDescription));

					AddAction->UpdateSearchData(FText::FromString(TaskName), FText::FromString(TaskDescription), FText::GetEmpty(), FText::GetEmpty());
					//AddAction->TaskClass = QuestTaskClass;

					UQuestGraphNode_Branch* Node = NewObject<UQuestGraphNode_Branch>(ContextMenuBuilder.OwnerOfTemporaries, UQuestGraphNode_Branch::StaticClass());
					UQuestBranch* NewBranch = NewObject<UQuestBranch>(QuestAsset);

					UQuestTask* DefaultTask = NewObject<UQuestTask>(NewBranch, QuestTaskClass);

					//Commented for now as SetID wont give a unique ID since outer uquest isn't valid yet.
					//NewBranch->SetID(QuestTaskClass->GetFName());

					NewBranch->QuestTasks.Add(DefaultTask);

					Node->Branch = NewBranch;
					Node->QuestNode = NewBranch;
					AddAction->NodeTemplate = Node;

					ActionMainCategory.Append(ActionCategoryBuilder);
				}
			}
			ContextMenuBuilder.Append(ActionMainCategory);
		}
	}

	if (!bIsState || bNoParent)
	{
		FCategorizedGraphActionListBuilder ActionGraphActionBuilder(TEXT("States:"));

		TSharedPtr<FQuestSchemaAction_NewNode> AddState = UQuestGraphSchema::AddNewNodeAction(ActionGraphActionBuilder, FText::GetEmpty(), FText::FromString("Add a state..."), FText::GetEmpty());
		UQuestGraphNode* Node = NewObject<UQuestGraphNode>(ContextMenuBuilder.OwnerOfTemporaries, UQuestGraphNode_State::StaticClass());
		AddState->NodeTemplate = Node;

		TSharedPtr<FQuestSchemaAction_NewNode> AddFailed = UQuestGraphSchema::AddNewNodeAction(ActionGraphActionBuilder, FText::GetEmpty(), FText::FromString("Fail the Quest..."), FText::GetEmpty());
		Node = NewObject<UQuestGraphNode>(ContextMenuBuilder.OwnerOfTemporaries, UQuestGraphNode_Failure::StaticClass());
		
		AddFailed->NodeTemplate = Node;

		TSharedPtr<FQuestSchemaAction_NewNode> AddSuccess = UQuestGraphSchema::AddNewNodeAction(ActionGraphActionBuilder, FText::GetEmpty(), FText::FromString("Succeed the Quest..."), FText::GetEmpty());
		Node = NewObject<UQuestGraphNode>(ContextMenuBuilder.OwnerOfTemporaries, UQuestGraphNode_Success::StaticClass());
		AddSuccess->NodeTemplate = Node;

		ContextMenuBuilder.Append(ActionGraphActionBuilder); 
	}

	if (bNoParent)
	{
		FCategorizedGraphActionListBuilder CommentActionBuilder(TEXT("Comments"));
		TSharedPtr<FQuestSchemaAction_AddComment> AddComment = UQuestGraphSchema::AddCommentAction(ContextMenuBuilder, FText::FromString("Add Comment..."), FText::GetEmpty());
		ContextMenuBuilder.Append(CommentActionBuilder);
	}

}

void UQuestGraphSchema::GetGraphDisplayInformation(const UEdGraph& Graph, FGraphDisplayInfo& DisplayInfo) const
{
	if (GetGraphType(&Graph) == GT_MAX)
	{
		DisplayInfo.PlainName = FText::FromString(Graph.GetName());
		DisplayInfo.DisplayName = DisplayInfo.PlainName;
		DisplayInfo.Tooltip = LOCTEXT("GraphTooltip_QuestGraph", "Graph used to define the quests layout.");
	}
	else
	{
		Super::GetGraphDisplayInformation(Graph, DisplayInfo);
	}
}

TSharedPtr<FQuestSchemaAction_NewNode> UQuestGraphSchema::AddNewNodeAction(FGraphActionListBuilderBase& ContextMenuBuilder, const FText& Category, const FText& MenuDesc, const FText& Tooltip)
{
	TSharedPtr<FQuestSchemaAction_NewNode> NewAction = TSharedPtr<FQuestSchemaAction_NewNode>(new FQuestSchemaAction_NewNode(Category, MenuDesc, Tooltip, 0));
	ContextMenuBuilder.AddAction(NewAction);

	return NewAction;
}

EGraphType UQuestGraphSchema::GetGraphType(const UEdGraph* TestEdGraph) const
{
	return GT_MAX; //TODO figure out how to add a GT_Quest type, maybe by casting uint8 ->EGraphType or something? Engine probably should have left some custom flags open for this 
}

FLinearColor UQuestGraphSchema::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
	return FLinearColor::White;
}

TSharedPtr<FEdGraphSchemaAction> UQuestGraphSchema::GetCreateCommentAction() const
{
	return TSharedPtr<FEdGraphSchemaAction>(static_cast<FEdGraphSchemaAction*>(new FQuestSchemaAction_AddComment));
}

UEdGraphNode* FQuestSchemaAction_AddComment::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode /*= true*/)
{
	UEdGraphNode_Comment* const CommentTemplate = NewObject<UEdGraphNode_Comment>();

	FVector2D SpawnLocation = Location;

	FQuestGraphEditor* QuestEditor = nullptr;

	if (UQuestBlueprint* const QuestAsset = Cast<UQuestBlueprint>(ParentGraph->GetOuter()))
	{
		if (UAssetEditorSubsystem* AESubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>())
		{
			QuestEditor = static_cast<FQuestGraphEditor*>(AESubsystem->FindEditorForAsset(QuestAsset, false));
		}
	}

	FSlateRect Bounds;
	if (QuestEditor && QuestEditor->Quest_GetBoundsForSelectedNodes(Bounds, 50.0f))
	{
		CommentTemplate->SetBounds(Bounds);
		SpawnLocation.X = CommentTemplate->NodePosX;
		SpawnLocation.Y = CommentTemplate->NodePosY;
	}

	UEdGraphNode* const NewNode = FEdGraphSchemaAction_NewNode::SpawnNodeFromTemplate<UEdGraphNode_Comment>(ParentGraph, CommentTemplate, SpawnLocation, bSelectNewNode);

	return NewNode;
}

TSharedPtr<FQuestSchemaAction_AddComment> UQuestGraphSchema::AddCommentAction(FGraphActionListBuilderBase& ContextMenuBuilder, const FText& MenuDesc, const FText& Tooltip)
{
	TSharedPtr<FQuestSchemaAction_AddComment> NewComment = TSharedPtr<FQuestSchemaAction_AddComment>(new FQuestSchemaAction_AddComment(MenuDesc, Tooltip));
	ContextMenuBuilder.AddAction(NewComment);
	return NewComment;
}
