// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class LQTL_Plugins : ModuleRules
{
	public LQTL_Plugins(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);


		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);


		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"UnrealEd",
				"Kismet",
				// ... add other public dependencies that you statically link with here ...
			}
			);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"ApplicationCore",
				"Engine",
				"RenderCore",
				"InputCore",
				"Slate",
				"SlateCore",
				"LQTL_Plugins_Runtime",
				"AssetTools",
				"AssetDefinition",
				"UnrealEd",
				"Kismet",            // Để dùng Workflow
				"KismetCompiler", // Để dùng FKismetCompilerContext
                "KismetWidgets",
				"GraphEditor", // Để dùng SGraphPin
				"PropertyEditor", // Để dùng FPropertyEditorModule 	
				"BlueprintGraph",
				"UMG", // Để dùng Blueprint/UserWidget.h

				"Projects"
				// ... add private dependencies that you statically link with here ...	
			}
			);


		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
