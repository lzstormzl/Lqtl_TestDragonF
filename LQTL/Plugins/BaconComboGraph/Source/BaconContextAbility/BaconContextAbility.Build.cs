// © 2025 mrbaconvn. All Rights Reserved.

using UnrealBuildTool;

public class BaconContextAbility : ModuleRules
{
	public BaconContextAbility(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",  
				"GameplayTags",
				"GameplayAbilities", 
			});
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine", 
				"GameplayTasks",
			});
		
		if(!(Target.Version.MajorVersion == 5 && Target.Version.MinorVersion >= 5))
		{
			// StructUtils is merged into Unreal Source in 5.5
			PublicDependencyModuleNames.Add("StructUtils");
		}
	}
}
