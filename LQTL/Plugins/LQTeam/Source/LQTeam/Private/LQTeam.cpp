// Copyright Epic Games, Inc. All Rights Reserved.

#include "LQTeam.h"

#define LOCTEXT_NAMESPACE "FLQTeamModule"

void FLQTeamModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FLQTeamModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FLQTeamModule, LQTeam)