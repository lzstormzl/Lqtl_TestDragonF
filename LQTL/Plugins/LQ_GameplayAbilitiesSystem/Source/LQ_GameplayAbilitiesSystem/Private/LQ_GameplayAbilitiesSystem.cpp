// Copyright Epic Games, Inc. All Rights Reserved.

#include "LQ_GameplayAbilitiesSystem.h"

#define LOCTEXT_NAMESPACE "FLQ_GameplayAbilitiesSystemModule"

void FLQ_GameplayAbilitiesSystemModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FLQ_GameplayAbilitiesSystemModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FLQ_GameplayAbilitiesSystemModule, LQ_GameplayAbilitiesSystem)