#include "TWQuestEditorModule.h"

// Thư viện Unreal
#include "Modules/ModuleManager.h"
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "ISettingsContainer.h"
#include <ISettingsCategory.h>
#include <AssetRegistry/AssetRegistryModule.h>
#include "EdGraphUtilities.h"
#include "UObject/Class.h"
#include "KismetCompiler.h"

// Thư viện Plugin
#include "AssetTypeActions_QuestAsset.h"
#include "AssetTypeActions_QuestTask.h"
#include "QuestGraphEditor.h"
#include "QuestEditorStyle.h"
#include "SQuestGraphNode.h"
#include "QuestEditorSettings.h"

DEFINE_LOG_CATEGORY(LogQuestEditor);

const FName FTWQuestEditorModule::QuestEditorAppId(TEXT("QuestEditorApp"));

#define LOCTEXT_NAMESPACE "FTWQuestEditorModule"

uint32 FTWQuestEditorModule::GameAssetCategory;

class FGraphPanelNodeFactory_QuestGraph : public FGraphPanelNodeFactory
{
	virtual TSharedPtr<class SGraphNode> CreateNode(UEdGraphNode* Node) const override
	{
		if (UQuestGraphNode* QuestNode = Cast<UQuestGraphNode>(Node))
		{
			return SNew(SQuestGraphNode, QuestNode);
		}
		return NULL;	
	}
};

TSharedPtr<FGraphPanelNodeFactory> GraphPanelNodeFactory_QuestGraph;

void FTWQuestEditorModule::StartupModule()
{
	FQuestEditorStyle::Initialize();

    RegisterSettings();

    MenuExtensibilityManager = MakeShareable(new FExtensibilityManager);
	ToolBarExtensibilityManager = MakeShareable(new FExtensibilityManager);

    GraphPanelNodeFactory_QuestGraph = MakeShareable(new FGraphPanelNodeFactory_QuestGraph());
	FEdGraphUtilities::RegisterVisualNodeFactory(GraphPanelNodeFactory_QuestGraph);

    IAssetTools& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

    // Dialogue Module đã defined Tale Weavers Category nên ta dùng lại
    GameAssetCategory = AssetToolsModule.FindAdvancedAssetCategory(FName(TEXT("TaleWeavers")));

    // Đăng ký Quest Asset Type Action
    TSharedPtr<FAssetTypeActions_QuestAsset> QuestAssetTypeAction = MakeShareable(new FAssetTypeActions_QuestAsset(GameAssetCategory));
	QuestAssetTypeActions = QuestAssetTypeAction;
	AssetToolsModule.RegisterAssetTypeActions(QuestAssetTypeAction.ToSharedRef());

	// Đăng ký Quest Task Asset Type Action
	TSharedPtr<FAssetTypeActions_QuestTask> QuestTaskTypeAction = MakeShareable(new FAssetTypeActions_QuestTask(GameAssetCategory));
	QuestTaskTypeActions = QuestTaskTypeAction;
	AssetToolsModule.RegisterAssetTypeActions(QuestTaskTypeAction.ToSharedRef());

	// Đăng ký Social Condition Asset Type Action
	TSharedPtr<FAssetTypeActions_SocialCondition> SocialConditionTypeAction = MakeShareable(new FAssetTypeActions_SocialCondition(GameAssetCategory));
	SocialConditionTypeActions = SocialConditionTypeAction;
	AssetToolsModule.RegisterAssetTypeActions(SocialConditionTypeAction.ToSharedRef());

	//Đăng ký Narrative Event Asset Type Action
	TSharedPtr<FAssetTypeActions_NarrativeEvent> NarrativeEventTypeAction = MakeShareable(new FAssetTypeActions_NarrativeEvent(GameAssetCategory));
	NarrativeEventTypeActions = NarrativeEventTypeAction;
	AssetToolsModule.RegisterAssetTypeActions(NarrativeEventTypeAction.ToSharedRef());

	FKismetCompilerContext::RegisterCompilerForBP(UQuestBlueprint::StaticClass(), [](UBlueprint* InBlueprint, FCompilerResultsLog& InMessageLog, const FKismetCompilerOptions& InCompileOptions)
		{
			return MakeShared<FQuestBlueprintCompilerContext>(CastChecked<UQuestBlueprint>(InBlueprint), InMessageLog, InCompileOptions);
		});

	IKismetCompilerInterface& KismetCompilerModule = FModuleManager::LoadModuleChecked<IKismetCompilerInterface>("KismetCompiler");
	KismetCompilerModule.GetCompilers().Add(&QuestBlueprintCompiler);

	// Đăng ký details panel cho quest editor
	// FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	// PropertyModule.RegisterCustomClassLayout("QuestBranch", FOnGetDetailCustomizationInstance::CreateStatic(&FQuestEditorDetails::MakeInstance));
	// PropertyModule.RegisterCustomClassLayout("QuestState", FOnGetDetailCustomizationInstance::CreateStatic(&FQuestEditorDetails::MakeInstance));
	// PropertyModule.NotifyCustomizationModuleChanged();
}

void FTWQuestEditorModule::ShutdownModule()
{
	UnregisterSettings();

	ToolBarExtensibilityManager.Reset();
	MenuExtensibilityManager.Reset();

	if (GraphPanelNodeFactory_QuestGraph.IsValid())
	{
		FEdGraphUtilities::UnregisterVisualNodeFactory(GraphPanelNodeFactory_QuestGraph);
		GraphPanelNodeFactory_QuestGraph.Reset();
	}

	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		if (QuestAssetTypeActions.IsValid())
		{
			AssetToolsModule.UnregisterAssetTypeActions(QuestAssetTypeActions.ToSharedRef());
			//AssetToolsModule.UnregisterAssetTypeActions(QuestActionTypeActions.ToSharedRef());			
			AssetToolsModule.UnregisterAssetTypeActions(QuestTaskTypeActions.ToSharedRef());
			AssetToolsModule.UnregisterAssetTypeActions(SocialConditionTypeActions.ToSharedRef());
			AssetToolsModule.UnregisterAssetTypeActions(NarrativeEventTypeActions.ToSharedRef());
		}
	}


	FQuestEditorStyle::Shutdown();
}

TSharedRef<IQuestEditor> FTWQuestEditorModule::CreateQuestEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, class UQuestBlueprint* QuestAsset)
{

	TSharedRef< FQuestGraphEditor > NewQuestEditor(new FQuestGraphEditor());
	NewQuestEditor->InitQuestEditor(Mode, InitToolkitHost, QuestAsset);
	return NewQuestEditor;
}

void FTWQuestEditorModule::RegisterSettings()
{
	// Registering some settings is just a matter of exposing the default UObject of
		// your desired class, feel free to add here all those settings you want to expose
		// to your LDs or artists.

	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		// Register the settings
		SettingsModule->RegisterSettings(
			"Project", 
			"Plugins", 
			"Tale Weavers Quests - Editor",
			FText::FromString("Tale Weavers Quests - Editor"),
			FText::FromString("Configuration Settings cho Tale Weavers Quest Editor"),
			GetMutableDefault<UQuestEditorSettings>()
		);

		// // Register the runtime settings
		// SettingsModule->RegisterSettings("Project", "Plugins", "Narrative Quests - Gameplay",
		// 	LOCTEXT("NarrativeRuntimeQuestSettingsName", "Narrative Quests - Gameplay"),
		// 	LOCTEXT("NarrativeRuntimeQuestSettingsDescription", "Configuration Settings for the Narrative Quest Runtime"),
		// 	GetMutableDefault<UTWQuestSettings>()
		// );
	}
}

void FTWQuestEditorModule::UnregisterSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "Tale Weavers Quests - Editor");
	}
} 

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FTWQuestEditorModule, TWQuestEditorModule)