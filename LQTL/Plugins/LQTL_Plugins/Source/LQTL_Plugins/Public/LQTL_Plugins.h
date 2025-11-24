// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Styling/SlateStyle.h"

// Thư viện Dialogue Plugins
#include "DialogueBlueprintCompiler.h"

class FLQTL_PluginsModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void RegisterSettings();
	void UnregisterSettings();

	uint32 GameAssetCategory;

private:
	TSharedPtr<class FAssetTypeActions_Base> _dialogueAssetTypeActions;

	FDialogueBlueprintCompiler DialogueBlueprintCompiler;
};
