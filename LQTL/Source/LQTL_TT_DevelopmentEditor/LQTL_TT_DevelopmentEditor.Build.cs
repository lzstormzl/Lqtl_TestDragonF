// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class LQTL_TT_DevelopmentEditor : ModuleRules
{
	public LQTL_TT_DevelopmentEditor(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "BlueprintGraph",
            "Kismet",
            "KismetCompiler",
            "GraphEditor",
        });
    }
}
