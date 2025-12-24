using UnrealBuildTool;

public class BitmaskUtils : ModuleRules
{
    public BitmaskUtils(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "Engine",
            "CoreUObject",
            "UnrealEd",
            "BlueprintGraph",
            "GraphEditor",
            "PropertyEditor",
            "InputCore",
            "SlateCore",
            "Slate",
            "EditorStyle",
            "Kismet",
            "KismetCompiler"
        });
    }
}