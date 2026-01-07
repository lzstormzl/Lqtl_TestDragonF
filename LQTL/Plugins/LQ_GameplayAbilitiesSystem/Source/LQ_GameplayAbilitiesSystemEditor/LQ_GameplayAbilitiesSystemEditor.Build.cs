// Copyright Tale Weavers

using UnrealBuildTool;

public class LQ_GameplayAbilitiesSystemEditor : ModuleRules
{
	public LQ_GameplayAbilitiesSystemEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"LQ_GameplayAbilitiesSystem"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"UnrealEd",
				"DataTableEditor",
				"GameplayAbilities",
				"GameplayTags",
				"ToolMenus",
				"InputCore"
			}
		);
	}
}
