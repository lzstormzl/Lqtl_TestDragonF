#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

#include "QuestBlueprintCompiler.h"
#include "IQuestEditor.h"

DECLARE_LOG_CATEGORY_EXTERN(LogQuestEditor, All, All);

// Quest Editor Module
class FTWQuestEditorModule : public IModuleInterface,
    public IHasMenuExtensibility, public IHasToolBarExtensibility
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

    void RegisterSettings();
	void UnregisterSettings();

	static uint32 GameAssetCategory;

	virtual TSharedRef<IQuestEditor> CreateQuestEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, class UQuestBlueprint* QuestAsset);

	/** Needed to allow toolbar/menu extensibility */
	virtual TSharedPtr<FExtensibilityManager> GetMenuExtensibilityManager() override { return MenuExtensibilityManager; }
	virtual TSharedPtr<FExtensibilityManager> GetToolBarExtensibilityManager() override { return ToolBarExtensibilityManager; }

	/** Quest editor app identifier string */
	static const FName QuestEditorAppId;

private:

	class UQuestEditorSettings* SettingsPtr;

	TSharedPtr<class FAssetTypeActions_Base> QuestAssetTypeActions;
	//TSharedPtr<class FAssetTypeActions_Base> QuestActionTypeActions;
	TSharedPtr<class FAssetTypeActions_Base> QuestTaskTypeActions;
	TSharedPtr<class FAssetTypeActions_Base> SocialConditionTypeActions;
	TSharedPtr<class FAssetTypeActions_Base> NarrativeEventTypeActions;

	TSharedPtr<FExtensibilityManager> MenuExtensibilityManager;
	TSharedPtr<FExtensibilityManager> ToolBarExtensibilityManager;


	/** Compiler customization for Widgets */
	FQuestBlueprintCompiler QuestBlueprintCompiler;
};