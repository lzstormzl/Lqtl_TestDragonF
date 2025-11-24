// Copyright Epic Games, Inc. All Rights Reserved.

#include "LQTL_Plugins_Runtime.h"

#define LOCTEXT_NAMESPACE "FLQTL_Plugins_RuntimeModule"

void FLQTL_Plugins_RuntimeModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FLQTL_Plugins_RuntimeModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FLQTL_Plugins_RuntimeModule, LQTL_Plugins_Runtime)