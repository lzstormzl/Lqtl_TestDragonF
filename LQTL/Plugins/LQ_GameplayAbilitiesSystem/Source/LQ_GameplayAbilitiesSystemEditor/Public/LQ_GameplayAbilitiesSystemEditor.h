// Copyright Tale Weavers

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FLQAttributeTableEditorExtension;

class FLQ_GameplayAbilitiesSystemEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	TSharedPtr<FLQAttributeTableEditorExtension> DataTableExtension;
};
