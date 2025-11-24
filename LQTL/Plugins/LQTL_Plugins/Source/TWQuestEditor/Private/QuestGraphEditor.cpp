#include "QuestGraphEditor.h"

// Thư viện Unreal
#include "ScopedTransaction.h"
#include <Engine/World.h>
#include "Kismet2/BlueprintEditorUtils.h"
#include <SBlueprintEditorToolbar.h>
#include "LevelEditor.h"
#include <AssetRegistry/AssetRegistryModule.h> 
#include "Kismet2/DebuggerCommands.h"
#include "WorkflowOrientedApp/WorkflowUObjectDocuments.h"

// Thư viện Plugin
#include "TWQuestEditorModule.h"
#include "QuestEditorModes.h"
#include "Quest/Quest.h"
#include "QuestNodeUserWidget.h"
#include "QuestGraph.h"
#include "QuestGraphSchema.h"
#include "QuestTaskBlueprint.h"

#define LOCTEXT_NAMESPACE "QuestAssetEditor"

const FName FQuestGraphEditor::QuestEditorMode(TEXT("QuestEditor"));

FQuestGraphEditor::FQuestGraphEditor()
{
	UEditorEngine* Editor = (UEditorEngine*)GEngine;
	if (Editor != NULL)
	{
		Editor->RegisterForUndo(this);
	}
}

FQuestGraphEditor::~FQuestGraphEditor()
{
	UEditorEngine* Editor = (UEditorEngine*)GEngine;
	if (Editor)
	{
		Editor->UnregisterForUndo(this);
	}
}

void FQuestGraphEditor::OnSelectedNodesChangedImpl(const TSet<class UObject*>& NewSelection)
{
	if (NewSelection.Num() == 1)
	{
		for (auto& Obj : NewSelection)
		{
			//Want to edit the underlying quest object, not the graph node
			if (UQuestGraphNode* GraphNode = Cast<UQuestGraphNode>(Obj))
			{
				TSet<class UObject*> ModifiedSelection;
				ModifiedSelection.Add(GraphNode->QuestNode);
				FBlueprintEditor::OnSelectedNodesChangedImpl(ModifiedSelection);
				return;
			}
		}
	}


	FBlueprintEditor::OnSelectedNodesChangedImpl(NewSelection);
}

void FQuestGraphEditor::RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	DocumentManager->SetTabManager(InTabManager);

	FWorkflowCentricApplication::RegisterTabSpawners(InTabManager);
}

void FQuestGraphEditor::InitQuestEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, class UQuestBlueprint* InQuestAsset)
{

	QuestBlueprint = InQuestAsset;

	if (!Toolbar.IsValid())
	{
		Toolbar = MakeShareable(new FBlueprintEditorToolbar(SharedThis(this)));
	}
	

	GetToolkitCommands()->Append(FPlayWorldCommands::GlobalPlayWorldActions.ToSharedRef());


	CreateDefaultCommands();
	//BindCommands();	
	RegisterMenus();

	//Needs called before InitAssetEditor() to avoid nullptr. TODO look into how anim blueprints don't use a CreateInternalWidgets() function
	CreateInternalWidgets();

	TSharedPtr<FQuestGraphEditor> ThisPtr(SharedThis(this));

	TArray<UObject*> ObjectsToEdit;
	ObjectsToEdit.Add(QuestBlueprint);

	const bool bCreateDefaultStandaloneMenu = true;
	const bool bCreateDefaultToolbar = true;
	InitAssetEditor(Mode, 
		InitToolkitHost, 
		FTWQuestEditorModule::QuestEditorAppId, 
		FTabManager::FLayout::NullLayout, 
		bCreateDefaultStandaloneMenu, 
		true, 
		ObjectsToEdit);

	//We need to initialize the document manager
	//if (!DocumentManager.IsValid())
	//{
	//	DocumentManager = MakeShareable(new FDocumentTracker);
	//	DocumentManager->Initialize(ThisPtr);

	//	//Register our graph editor tab with the factory
	//	TSharedRef<FDocumentTabFactory> GraphEditorFactory = MakeShareable(new FQuestGraphEditorSummoner(ThisPtr,
	//		FQuestGraphEditorSummoner::FOnCreateGraphEditorWidget::CreateSP(this, &FQuestGraphEditor::CreateGraphEditorWidget)
	//	));

	//	GraphEditorTabFactoryPtr = GraphEditorFactory;
	//	DocumentManager->RegisterDocumentFactory(GraphEditorFactory);
	//}

	TArray<UBlueprint*> EditedBlueprints;
	EditedBlueprints.Add(QuestBlueprint);

	CommonInitialization(EditedBlueprints, false);

	AddApplicationMode(QuestEditorMode, MakeShareable(new FQuestEditorApplicationMode(SharedThis(this))));

	//ExtendMenu();
	//ExtendToolbar();
	RegenerateMenusAndToolbars();

	SetCurrentMode(QuestEditorMode);

	PostLayoutBlueprintEditorInitialization();

	
	//Load all Quest Tasks into memory since we need them for editing quests 
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> AssetData;

	FARFilter Filter;
	Filter.bRecursiveClasses = true;
	Filter.bRecursivePaths = true;
	Filter.bIncludeOnlyOnDiskAssets = false;

// #if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 0
// 	Filter.TagsAndValues.Add("ParentClass", FString("/Script/CoreUObject.Class'/Script/LQTL_Plugins_Runtime.QuestTask'"));
// 	Filter.ClassPaths.Add(FTopLevelAssetPath("Class'/Script/Engine.Blueprint'"));
// #else
// 	Filter.TagsAndValues.Add("ParentClass", FString("Class'/Script/LQTL_Plugins_Runtime.QuestTask'"));
// 	Filter.ClassNames.Add("Blueprint");
// #endif

	AssetRegistryModule.Get().GetAssets(Filter, AssetData);

	for (auto& Asset : AssetData)
	{
		if (UQuestTaskBlueprint* BPAsset = Cast<UQuestTaskBlueprint>(Asset.GetAsset()))
		{
			BPAsset->GetClass()->GetDefaultObject();
		}
	}

	FLevelEditorModule& LevelEditor = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	LevelEditor.OnMapChanged().AddRaw(this, &FQuestGraphEditor::OnWorldChange);

}

FName FQuestGraphEditor::GetToolkitFName() const
{
	return FName("Quest Editor");
}

FText FQuestGraphEditor::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "QuestEditor");
}

FString FQuestGraphEditor::GetWorldCentricTabPrefix() const
{
	return "QuestEditor";
}

FLinearColor FQuestGraphEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0.0f, 0.0f, 0.2f, 0.5f);
}

FText FQuestGraphEditor::GetToolkitToolTipText() const
{
	if (QuestBlueprint)
	{
		return FAssetEditorToolkit::GetToolTipTextForObject(QuestBlueprint);
	}
	return FText();
}

UBlueprint* FQuestGraphEditor::GetBlueprintObj() const
{
	const TArray<UObject*>& EditingObjs = GetEditingObjects();
	for (int32 i = 0; i < EditingObjs.Num(); ++i)
	{
		if (EditingObjs[i]->IsA<UQuestBlueprint>()) { return (UBlueprint*)EditingObjs[i]; }
	}
	return nullptr;
}

UQuestBlueprint* FQuestGraphEditor::GetQuestAsset() const
{
	return QuestBlueprint;
}

void FQuestGraphEditor::CreateInternalWidgets()
{
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
	DetailsView->OnFinishedChangingProperties().AddSP(this, &FQuestGraphEditor::OnFinishedChangingProperties);
}

void FQuestGraphEditor::PostUndo(bool bSuccess)
{
	if (bSuccess)
	{
	}

	FBlueprintEditor::PostUndo(bSuccess);

	if (QuestBlueprint->QuestGraph)
	{
		// Update UI
		QuestBlueprint->QuestGraph->NotifyGraphChanged();
		if (QuestBlueprint)
		{
			QuestBlueprint->PostEditChange();
			QuestBlueprint->MarkPackageDirty();
		}
	}

}

void FQuestGraphEditor::PostRedo(bool bSuccess)
{
	if (bSuccess)
	{
	}

	FBlueprintEditor::PostRedo(bSuccess);

	// Update UI
	if (QuestBlueprint->QuestGraph)
	{
		QuestBlueprint->QuestGraph->NotifyGraphChanged();
		if (QuestBlueprint)
		{
			QuestBlueprint->PostEditChange();
			QuestBlueprint->MarkPackageDirty();
		}
	}
}

bool FQuestGraphEditor::CanAccessQuestEditorMode() const
{
	return IsValid(QuestBlueprint);
}

FText FQuestGraphEditor::GetLocalizedMode(FName InMode)
{
	static TMap< FName, FText > LocModes;

	if (LocModes.Num() == 0)
	{
		LocModes.Add(QuestEditorMode, LOCTEXT("QuestEditorMode", "Quest Graph"));
	}

	check(InMode != NAME_None);
	const FText* OutDesc = LocModes.Find(InMode);
	check(OutDesc);
	return *OutDesc;
}

void FQuestGraphEditor::OnWorldChange(UWorld* World, EMapChangeType MapChangeType)
{

	//Dialogue graph nodes will be referencing the UWorld, and if it changes this will breakcc
	if (World)
	{
		for (TObjectIterator<UUserWidget> Itr; Itr; ++Itr)
		{
			UUserWidget* Widget = *Itr;

			if (Widget->IsA<UQuestNodeUserWidget>())
			{
				Widget->Rename(nullptr, GetTransientPackage());
			}
		}
	}
}

TSharedRef<class SGraphEditor> FQuestGraphEditor::CreateQuestGraphEditorWidget(UEdGraph* InGraph)
{
	check(InGraph);

	//Register commands for the quest graph 
	//CreateQuestGraphCommandList();

	SGraphEditor::FGraphEditorEvents InEvents;
	InEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(this, &FQuestGraphEditor::OnSelectedNodesChanged);
	InEvents.OnNodeDoubleClicked = FSingleNodeEvent::CreateSP(this, &FQuestGraphEditor::OnQuestNodeDoubleClicked);
	InEvents.OnTextCommitted = FOnNodeTextCommitted::CreateSP(this, &FQuestGraphEditor::OnNodeTitleCommitted);

	// Make title bar
	TSharedRef<SWidget> TitleBarWidget =
		SNew(SBorder)
		.BorderImage(FAppStyle::Get().GetBrush(TEXT("Graph.TitleBackground")))
		.HAlign(HAlign_Fill)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		.HAlign(HAlign_Center)
		.FillWidth(1.f)
		[
			SNew(STextBlock)
			.Text(this, &FQuestGraphEditor::GetQuestEditorTitle)
			.TextStyle(FAppStyle::Get(), TEXT("GraphBreadcrumbButtonText"))
		]
		];

	// Make full graph editor
	const bool bGraphIsEditable = InGraph->bEditable;
	return SNew(SGraphEditor)
		.AdditionalCommands(GraphEditorCommands)
		.IsEditable(this, &FQuestGraphEditor::InEditingMode, bGraphIsEditable)
		.Appearance(this, &FQuestGraphEditor::GetQuestGraphAppearance)
		.TitleBar(TitleBarWidget)
		.GraphToEdit(InGraph)
		.GraphEvents(InEvents);	
}

FGraphAppearanceInfo FQuestGraphEditor::GetQuestGraphAppearance() const
{
	FGraphAppearanceInfo AppearanceInfo;
	AppearanceInfo.CornerText = LOCTEXT("AppearanceCornerText", "QUEST EDITOR");
	return AppearanceInfo;
}

FText FQuestGraphEditor::GetQuestEditorTitle() const
{
	if (UQuest* QuestObj = Cast<UQuest>(GetBlueprintObj()->GeneratedClass->GetDefaultObject()))
	{
		return QuestObj->GetQuestName();
	}

	return FText::GetEmpty();
}

bool FQuestGraphEditor::InEditingMode(bool bGraphIsEditable) const
{
	return bGraphIsEditable;
}

void FQuestGraphEditor::RestoreQuestGraph()
{
	if (!QuestBlueprint)
	{
		return;
	}
	UQuestGraph* MyGraph = Cast<UQuestGraph>(QuestBlueprint->QuestGraph);
	const bool bNewGraph = MyGraph == NULL;
	if (MyGraph == NULL)
	{
		// Tạo ra một Quest Graph mới
		QuestBlueprint->QuestGraph = FBlueprintEditorUtils::CreateNewGraph(QuestBlueprint, TEXT("Tale Weavers Quest Graph"), UQuestGraph::StaticClass(), UQuestGraphSchema::StaticClass());
		MyGraph = Cast<UQuestGraph>(QuestBlueprint->QuestGraph);

		// Thêm graph vào Blueprint
		FBlueprintEditorUtils::AddUbergraphPage(QuestBlueprint, MyGraph);

		// TODO: Tạo node (root) mặc định cho graph
		const UEdGraphSchema* Schema = MyGraph->GetSchema();
		Schema->CreateDefaultNodesForGraph(*MyGraph);

		MyGraph->OnCreated();
	}
	else
	{
		MyGraph->OnLoaded();
	}

	MyGraph->Initialize();

	TSharedRef<FTabPayload_UObject> Payload = FTabPayload_UObject::Make(MyGraph);
	TSharedPtr<SDockTab> DocumentTab = DocumentManager->OpenDocument(Payload, bNewGraph ? FDocumentTracker::OpenNewDocument : FDocumentTracker::RestorePreviousDocument);

	if (QuestBlueprint->LastEditedDocuments.Num() > 0)
	{
		TSharedRef<SGraphEditor> GraphEditor = StaticCastSharedRef<SGraphEditor>(DocumentTab->GetContent());
		
		GraphEditor->SetViewLocation(QuestBlueprint->LastEditedDocuments[0].SavedViewOffset, QuestBlueprint->LastEditedDocuments[0].SavedZoomAmount);


	}
}

void FQuestGraphEditor::OnQuestNodeDoubleClicked(UEdGraphNode* Node)
{
	// if (UQuestGraphNode* QuestGraphNode = Cast<UQuestGraphNode>(Node))
	// {
	// 	//Something went super wrong is state is invalid 
	// 	if (!QuestGraphNode->QuestNode)
	// 	{
	// 		return;
	// 	}

	// 	//Create a custom event for when we reach this state!
	// 	if (UEdGraph* EventGraph = FBlueprintEditorUtils::FindEventGraph(QuestBlueprint))
	// 	{
	// 		if (EventGraph->Nodes.Find(QuestGraphNode->OnEnteredCustomNode) < 0)
	// 		{
	// 			if (UFunction* StateFunc = QuestGraphNode->FindFunction(GET_FUNCTION_NAME_CHECKED(UQuestGraphNode, OnEntered)))
	// 			{
	// 				//Try use the ID of the node if we gave it one prior to event add
	// 				const FString NodeID = (QuestGraphNode->QuestNode->GetID() != NAME_None ? QuestGraphNode->QuestNode->GetID().ToString() : QuestGraphNode->QuestNode->GetName());
	// 				FString OnEnteredFuncName = "On Activated/Deactivated - " + NodeID;

	// 				OnEnteredFuncName.RemoveSpacesInline();

	// 				if (UK2Node_CustomEvent* OnEnteredEvent = UK2Node_CustomEvent::CreateFromFunction(EventGraph->GetGoodPlaceForNewNode(), EventGraph, OnEnteredFuncName, StateFunc, false))
	// 				{
	// 					if (QuestGraphNode->QuestNode->IsA<UQuestState>())
	// 					{
	// 						OnEnteredEvent->NodeComment = FString::Printf(TEXT("This event will be called automatically when the %s state is entered/exited. Use the bActivated param to check which occured."), *NodeID);
	// 					}
	// 					else
	// 					{
	// 						OnEnteredEvent->NodeComment = FString::Printf(TEXT("This event will be called automatically when the %s branch becomes active, or was completed (and thus deactivated). Use the bActivated param to check which occured."), *NodeID);
	// 					}

	// 					OnEnteredEvent->SetMakeCommentBubbleVisible(true);

	// 					QuestGraphNode->OnEnteredCustomNode = OnEnteredEvent;
	// 					QuestGraphNode->QuestNode->OnEnteredFuncName = FName(OnEnteredFuncName);

	// 					OnEnteredEvent->bCanRenameNode = OnEnteredEvent->bIsEditable = false;

	// 					//Jump the new node we made! 
	// 					if (OnEnteredEvent)
	// 					{
	// 						JumpToNode(OnEnteredEvent, false);
	// 					}
	// 				}
	// 			}
	// 		}
	// 		else
	// 		{
	// 			//Event graph already has the node created, jump to it
	// 			if (QuestGraphNode->OnEnteredCustomNode)
	// 			{
	// 				JumpToNode(QuestGraphNode->OnEnteredCustomNode, false);
	// 			}
	// 		}
	// 	}
	// }
}

bool FQuestGraphEditor::Quest_GetBoundsForSelectedNodes(class FSlateRect& Rect, float Padding)
{
	const bool bResult = FBlueprintEditor::GetBoundsForSelectedNodes(Rect, Padding);

	return bResult;
}

void FQuestGraphEditor::CreateDefaultCommands()
{
	//FQuestEditorCommands::Register();
	//FBlueprintEditorCommand::Register();

	// ToolkitCommands->MapAction(FQuestEditorCommands::Get().ShowQuestDetails,
	// 	FExecuteAction::CreateSP(this, &FQuestGraphEditor::ShowQuestDetails),
	// 	FCanExecuteAction::CreateSP(this, &FQuestGraphEditor::CanShowQuestDetails));

	// ToolkitCommands->MapAction(FQuestEditorCommands::Get().ViewTutorial,
	// 	FExecuteAction::CreateSP(this, &FQuestGraphEditor::OpenNarrativeTutorialsInBrowser),
	// 	FCanExecuteAction::CreateSP(this, &FQuestGraphEditor::CanOpenNarrativeTutorialsInBrowser));

	// ToolkitCommands->MapAction(FQuestEditorCommands::Get().QuickAddNode,
	// 	FExecuteAction::CreateSP(this, &FQuestGraphEditor::QuickAddNode),
	// 	FCanExecuteAction::CreateSP(this, &FQuestGraphEditor::CanQuickAddNode));

	FBlueprintEditor::CreateDefaultCommands();
}

TSharedRef<SWidget> FQuestGraphEditor::SpawnProperties()
{
	return
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		.HAlign(HAlign_Fill)
		[
			DetailsView.ToSharedRef()
		];
}