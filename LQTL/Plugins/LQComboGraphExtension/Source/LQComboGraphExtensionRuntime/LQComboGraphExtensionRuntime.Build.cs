using UnrealBuildTool;

public class LQComboGraphExtensionRuntime : ModuleRules
{
	public LQComboGraphExtensionRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core", "ComboGraphRuntime", "LQ_GameplayAbilitiesSystem", "GameplayAbilities", "ComboGraphASC"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore"
			}
		);
	}
}