// Copyright(c) Aurora Devs 2022-2025. All Rights Reserved.

using UnrealBuildTool;

public class AuroraDevs_UGCEditor : ModuleRules
{
	public AuroraDevs_UGCEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		ShadowVariableWarningLevel = WarningLevel.Error;
		bUseUnity = false;
		IWYUSupport = IWYUSupport.Full;
		PrecompileForTargets = PrecompileTargetsType.Any;

		PrivateIncludePaths.Add(ModuleDirectory);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"BlueprintGraph",
				"AuroraDevs_UGC"
			});
	}
}
