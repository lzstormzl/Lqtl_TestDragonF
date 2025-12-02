// Copyright Epic Games, Inc. All Rights Reserved.

#include "LQTL_Plugins.h"
#include <AssetRegistry/AssetRegistryModule.h>
#include "Styling/SlateStyleRegistry.h"
#include "Interfaces/IPluginManager.h"
#include "EdGraphUtilities.h"
#include "SGraphPin.h"
#include <ISettingsCategory.h>
#include "ISettingsModule.h"
#include "IAssetTools.h"
#include "AssetToolsModule.h"
#include "ISettingsModule.h"
#include "KismetCompiler.h"

// Thư viện Dialogue Plugins
#include "AssetTypeActions_DialogueBlueprint.h"
#include "DialogueGraphNode.h"
#include "STWDialogueGraphNode.h"
#include "TWDialogueEditorSettings.h"
#include "DialogueBlueprint.h"
#include "DialogueBlueprintCompiler.h"

#define LOCTEXT_NAMESPACE "FLQTL_PluginsModule"

class FGraphPanelNodeFactory_DialogueGraph : public FGraphPanelNodeFactory
{
	virtual TSharedPtr<class SGraphNode> CreateNode(UEdGraphNode* Node) const override
	{
		if (UDialogueGraphNode* DialogueNode = Cast<UDialogueGraphNode>(Node))
		{
			return SNew(STWDialogueGraphNode, DialogueNode);
		}
		return NULL;
	}
}; 

TSharedPtr<FGraphPanelNodeFactory> GraphPanelNodeFactory_DialogueGraph;

void FLQTL_PluginsModule::StartupModule()
{
	// Đăng ký Settings
	RegisterSettings();

	// Đăng ký Node Visual
	GraphPanelNodeFactory_DialogueGraph = MakeShareable(new FGraphPanelNodeFactory_DialogueGraph());
	FEdGraphUtilities::RegisterVisualNodeFactory(GraphPanelNodeFactory_DialogueGraph);

	IAssetTools& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

	GameAssetCategory = AssetToolsModule.RegisterAdvancedAssetCategory(FName(TEXT("TaleWeavers")), LOCTEXT("TaleWeaversCategory", "Tale Weavers"));

	// Đăng ký Dialogue Asset Type Action
	TSharedPtr<FAssetTypeActions_DialogueBlueprint> DialogueAssetTypeAction = MakeShareable(new FAssetTypeActions_DialogueBlueprint(GameAssetCategory));
	_dialogueAssetTypeActions = DialogueAssetTypeAction;
	AssetToolsModule.RegisterAssetTypeActions(_dialogueAssetTypeActions.ToSharedRef());


	// Đăng ký Dialogue Blueprint Compiler
	FKismetCompilerContext::RegisterCompilerForBP(UDialogueBlueprint::StaticClass(), [](UBlueprint* InBlueprint, FCompilerResultsLog& InMessageLog, const FKismetCompilerOptions& InCompileOptions)
		{
			return MakeShared<FDialogueBlueprintCompilerContext>(CastChecked<UDialogueBlueprint>(InBlueprint), InMessageLog, InCompileOptions);
		});

	IKismetCompilerInterface& KismetCompilerModule = FModuleManager::LoadModuleChecked<IKismetCompilerInterface>("KismetCompiler");
	KismetCompilerModule.GetCompilers().Add(&DialogueBlueprintCompiler);

	//  This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FLQTL_PluginsModule::ShutdownModule()
{
	// Hủy đăng ký Settings
	UnregisterSettings();

	// Hủy đăng ký Node Visual
	if(GraphPanelNodeFactory_DialogueGraph.IsValid()){

		FEdGraphUtilities::UnregisterVisualNodeFactory(GraphPanelNodeFactory_DialogueGraph);
	}

	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		//Hủy đăng ký Dialogue Asset Type Action
		if (_dialogueAssetTypeActions.IsValid())
		{
			AssetToolsModule.UnregisterAssetTypeActions(_dialogueAssetTypeActions.ToSharedRef());
		}
	}
}

void FLQTL_PluginsModule::RegisterSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		//Register settings
		SettingsModule->RegisterSettings(
			"Project", 
			"Plugins", 
			"Tale Weavers Dialogues - Editor", 
			FText::FromString("Tale Weavers Dialogue - Editor"), 
			FText::FromString("Tùy chỉnh settings cho Dialogue Editor"), 
			GetMutableDefault<UTWDialogueEditorSettings>()
		);

	}
}

void FLQTL_PluginsModule::UnregisterSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "Tale Weavers Dialogues - Editor");
		
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FLQTL_PluginsModule, LQTL_Plugins)