// © 2025 mrbaconvn. All Rights Reserved.

using UnrealBuildTool;

public class ComboGraphEditor : ModuleRules
{
	public ComboGraphEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs; 
	
		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject",
			"Engine",
		});

		PrivateDependencyModuleNames.AddRange(new string[] {
			"InputCore",
            "Slate",
            "SlateCore",
            "UnrealEd",
            "AssetTools",
            "PropertyEditor",
            "ComboGraphRuntime",
			"Projects",
			"ToolMenus",
			"GraphEditor", 
			"GameplayTags",
			"ApplicationCore",
		});
	}
}
