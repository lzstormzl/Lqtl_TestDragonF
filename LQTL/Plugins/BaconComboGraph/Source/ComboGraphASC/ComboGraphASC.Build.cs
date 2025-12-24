// © 2025 mrbaconvn. All Rights Reserved.

using UnrealBuildTool;

public class ComboGraphASC : ModuleRules
{
	public ComboGraphASC(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject",
			"Engine",
			"ComboGraphRuntime", 
			"AnimationDrivenSystem", 
			"BaconContextAbility",
        });

		PrivateDependencyModuleNames.AddRange(new string[] { 
			"GameplayTags",
			"GameplayAbilities", 
			"GameplayTasks",
        });
		
		if(!(Target.Version.MajorVersion == 5 && Target.Version.MinorVersion >= 5))
		{
			// StructUtils is merged into Unreal Source in 5.5
			PublicDependencyModuleNames.Add("StructUtils");
		}
	}
}
