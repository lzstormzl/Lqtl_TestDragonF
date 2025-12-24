// © 2025 mrbaconvn. All Rights Reserved.

using UnrealBuildTool;

public class ComboGraphRuntime : ModuleRules
{
	public ComboGraphRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;  
	
		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core",
			"CoreUObject",
			"Engine",
        });

		PrivateDependencyModuleNames.AddRange(new string[] { 
			"GameplayTags",
        });
	}
}
