#include "DialogueGraphEditor.h"
#include <SBlueprintEditorToolbar.h> // Sử dụng FBlueprintEditorToolbar
#include <Kismet2/DebuggerCommands.h>
#include "IDetailsView.h"
#include "PropertyEditorModule.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "EdGraphSchema_K2_Actions.h"
#include "K2Node_CustomEvent.h"

// Thư viện Dialogue Plugins
#include "DialogueEditorModes.h"
#include "TWDialogueGraph.h"
#include "TWDialogueGraphSchema.h"
#include "DialogueGraphNode.h"

#define LOCTEXT_NAMESPACE "DialogueAssetEditor"

const FName FDialogueGraphEditor::DialogueEditorMode(TEXT("TWDialogueEditor"));

void FDialogueGraphEditor::OnSelectedNodesChangedImpl(const TSet<class UObject*>& NewSelection)
{
	if(NewSelection.Num() == 1) // Nếu chỉ selection 1 node
	{
		for (auto& Obj : NewSelection)
		{
			if (UDialogueGraphNode* GraphNode = Cast<UDialogueGraphNode>(Obj))
			{
				TSet<class UObject*> ModifiedSelection;
				check(GraphNode->TWDialogueNode);
				ModifiedSelection.Add(GraphNode->TWDialogueNode);
				FBlueprintEditor::OnSelectedNodesChangedImpl(ModifiedSelection);
				return;
			}
		}
	}

	FBlueprintEditor::OnSelectedNodesChangedImpl(NewSelection);
}

void FDialogueGraphEditor::RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	DocumentManager->SetTabManager(InTabManager);

	FWorkflowCentricApplication::RegisterTabSpawners(InTabManager);
}


void FDialogueGraphEditor::InitDialogueEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, class UDialogueBlueprint* InDialogue)
{
    DialogueBlueprint = InDialogue;

    UDialogueBlueprint* _dialogueAsset = Cast<UDialogueBlueprint>(InDialogue);

    if (!Toolbar.IsValid())
	{
		Toolbar = MakeShareable(new FBlueprintEditorToolbar(SharedThis(this)));
	}

    GetToolkitCommands()->Append(FPlayWorldCommands::GlobalPlayWorldActions.ToSharedRef());

    CreateDefaultCommands();
    //BindCommands();
    RegisterMenus();

    FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.bUpdatesFromSelection = false;
	DetailsViewArgs.bLockable = false;
	DetailsViewArgs.bAllowSearch = true;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
	DetailsViewArgs.bHideSelectionTip = false;
	DetailsViewArgs.NotifyHook = this;
	DetailsViewArgs.DefaultsOnlyVisibility = EEditDefaultsOnlyNodeVisibility::Hide;
	DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	DetailsView->SetObject(NULL);
	DetailsView->OnFinishedChangingProperties().AddSP(this, &FDialogueGraphEditor::OnFinishedChangingProperties);

    TSharedPtr<FDialogueGraphEditor> ThisPtr(SharedThis(this));

	TArray<UObject*> ObjectsToEdit;
	ObjectsToEdit.Add(DialogueBlueprint);
	
	
    InitAssetEditor(Mode, 
        InitToolkitHost,  
        TEXT("DialogueEditor"),
        FTabManager::FLayout::NullLayout, 
        true,
        true,
        ObjectsToEdit);

	TArray<UBlueprint*> EditedBlueprints;
	EditedBlueprints.Add(DialogueBlueprint);	
	
	CommonInitialization(EditedBlueprints, false);

    AddApplicationMode(DialogueEditorMode, MakeShareable(new FDialogueEditorMode(SharedThis(this))));

    RegenerateMenusAndToolbars();

    SetCurrentMode(DialogueEditorMode);

    PostLayoutBlueprintEditorInitialization();
}

void FDialogueGraphEditor::CreateDefaultCommands()
{
	FBlueprintEditor::CreateDefaultCommands();
}

FText FDialogueGraphEditor::GetLocalizedMode(FName InMode)
{
	static TMap< FName, FText > LocModes;

	if (LocModes.Num() == 0)
	{
		LocModes.Add(DialogueEditorMode, LOCTEXT("DialogueEditorMode", "Dialogue Graph"));
	}

	check(InMode != NAME_None);
	const FText* OutDesc = LocModes.Find(InMode);
	check(OutDesc);
	return *OutDesc;
}

void FDialogueGraphEditor::RestoreDialogueGraph()
{
	UTWDialogueGraph* DialogueGraph = Cast<UTWDialogueGraph>(DialogueBlueprint->DialogueGraph); 
	const bool bNewGraph = DialogueGraph == NULL;
	if (DialogueGraph == NULL)
	{
		DialogueBlueprint->DialogueGraph = FBlueprintEditorUtils::CreateNewGraph(
			DialogueBlueprint,
			TEXT("Tale Weavers Dialogue Graph"),
			UTWDialogueGraph::StaticClass(),
			UTWDialogueGraphSchema::StaticClass()   // Chỗ này cần tạo schema riêng cho nó để tạo node action context riêng
		);

		DialogueGraph = Cast<UTWDialogueGraph>(DialogueBlueprint->DialogueGraph);
		
		FBlueprintEditorUtils::AddUbergraphPage(DialogueBlueprint,DialogueGraph);

		// Tạo 1 Schema để tạo 1 cái Node Default cho graph này thường là Node root
		const UEdGraphSchema* Schema = DialogueGraph->GetSchema();
		Schema->CreateDefaultNodesForGraph(*DialogueGraph);

	}
}

bool FDialogueGraphEditor::Dialogue_GetBoundsForSelectedNodes(class FSlateRect& Rect, float Padding)
{
	const bool bResult = FBlueprintEditor::GetBoundsForSelectedNodes(Rect, Padding);

	return bResult;
}

void FDialogueGraphEditor::SetupGraphEditorEvents(UEdGraph* InGraph, SGraphEditor::FGraphEditorEvents& InEvents)
{
	FBlueprintEditor::SetupGraphEditorEvents(InGraph, InEvents);

	// Custom menu for K2 schemas
	if (InGraph->Schema != nullptr && InGraph->Schema->IsChildOf(UTWDialogueGraphSchema::StaticClass()))
	{
		InEvents.OnNodeDoubleClicked = FSingleNodeEvent::CreateSP(this, &FDialogueGraphEditor::OnDialogueNodeDoubleClicked);
	}
}

void FDialogueGraphEditor::OnDialogueNodeDoubleClicked(UEdGraphNode* Node)
{
	if (UDialogueGraphNode* DNode = Cast<UDialogueGraphNode>(Node))
	{
		if (DNode->TWDialogueNode)
		{
			if (UEdGraph* EventGraph = FBlueprintEditorUtils::FindEventGraph(DialogueBlueprint))
			{
				if (EventGraph->Nodes.Find(DNode->OnPlayedCustomNode) < 0)
				{
					if (UFunction* Func = DNode->FindFunction(GET_FUNCTION_NAME_CHECKED(UDialogueGraphNode, OnStartedOrFinished)))
					{
						//Try use the ID of the node if we gave it one prior to event add
						const FString NodeID = (DNode->TWDialogueNode->GetID() != NAME_None ? DNode->TWDialogueNode->GetID().ToString() : DNode->TWDialogueNode->GetName());
						FString OnEnteredFuncName = "OnDialogueNode Started/Finished Playing - " + NodeID;

						if (UK2Node_CustomEvent* OnPlayedEvent = UK2Node_CustomEvent::CreateFromFunction(EventGraph->GetGoodPlaceForNewNode(), EventGraph, OnEnteredFuncName, Func, false))
						{
							DNode->TWDialogueNode->OnPlayNodeFuncName = FName(OnEnteredFuncName);
							DNode->OnPlayedCustomNode = OnPlayedEvent;

							OnPlayedEvent->NodeComment = FString::Printf(TEXT("This event will automatically be called when this dialogue line starts/finishes. Use the bStarted param to check which occured."));
							OnPlayedEvent->SetMakeCommentBubbleVisible(true);

							OnPlayedEvent->bCanRenameNode = OnPlayedEvent->bIsEditable = false;

							//Jump to our newly created event! 
							JumpToNode(OnPlayedEvent, false);
						}
					}
				}
				else if(DNode->OnPlayedCustomNode)
				{
					JumpToNode(DNode->OnPlayedCustomNode, false);
				}
			}
		}
	}
}
