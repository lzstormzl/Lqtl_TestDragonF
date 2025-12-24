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

#define LOCTEXT_NAMESPACE "FLQTL_PluginsModule"

TSharedPtr<FGraphPanelNodeFactory> GraphPanelNodeFactory_DialogueGraph;

void FLQTL_PluginsModule::StartupModule()
{
	// Đăng ký Settings
	RegisterSettings();

	IAssetTools& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

	GameAssetCategory = AssetToolsModule.RegisterAdvancedAssetCategory(FName(TEXT("TaleWeavers")), LOCTEXT("TaleWeaversCategory", "Tale Weavers"));

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

	}
}

void FLQTL_PluginsModule::RegisterSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
// 		//Register settings
// 		SettingsModule->RegisterSettings(
// 			"Project", 
// 			"Plugins", 
// 			"Tale Weavers Dialogues - Editor", 
// 			FText::FromString("Tale Weavers Dialogue - Editor"), 
// 			FText::FromString("Tùy chỉnh settings cho Dialogue Editor"), 
// 			GetMutableDefault<UTWDialogueEditorSettings>()
// 		);

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