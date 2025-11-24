using UnrealBuildTool;

public class TWQuestEditor : ModuleRules
{
	public TWQuestEditor(ReadOnlyTargetRules Target) : base(Target)
	{
        bLegacyPublicIncludePaths = false;
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(new string[] {});
        PrivateIncludePaths.AddRange(new string[] { "TWQuestEditor/Private" });

        //Required for registering quest assets
        PrivateIncludePathModuleNames.AddRange(
            new string[] {
                "AssetRegistry",
                "AssetTools",
                "PropertyEditor",
                "ContentBrowser"
            }
            );

        PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core"
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
                "EditorStyle",
                "UnrealEd",
                "AudioEditor",
                "MessageLog",
                "GraphEditor",
                "Projects",
                "Kismet",
				"KismetCompiler",
                "KismetWidgets",
                "PropertyEditor",
                "AnimGraph",
                "BlueprintGraph",
                "ClassViewer",
                "LQTL_Plugins_Runtime",
                "AIGraph",
                "AssetTools",
                "UMG"
            }
			);
		

	}
}