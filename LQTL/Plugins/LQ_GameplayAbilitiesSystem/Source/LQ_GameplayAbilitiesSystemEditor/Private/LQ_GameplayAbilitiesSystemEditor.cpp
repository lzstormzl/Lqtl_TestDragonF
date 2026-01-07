// Copyright Tale Weavers

#include "LQ_GameplayAbilitiesSystemEditor.h"
#include "LQAttributeTableEditorExtension.h"

#define LOCTEXT_NAMESPACE "FLQ_GameplayAbilitiesSystemEditorModule"

void FLQ_GameplayAbilitiesSystemEditorModule::StartupModule()
{
	DataTableExtension = MakeShared<FLQAttributeTableEditorExtension>();
	DataTableExtension->Initialize();
}

void FLQ_GameplayAbilitiesSystemEditorModule::ShutdownModule()
{
	if (DataTableExtension.IsValid())
	{
		DataTableExtension->Shutdown();
		DataTableExtension.Reset();
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FLQ_GameplayAbilitiesSystemEditorModule, LQ_GameplayAbilitiesSystemEditor)
