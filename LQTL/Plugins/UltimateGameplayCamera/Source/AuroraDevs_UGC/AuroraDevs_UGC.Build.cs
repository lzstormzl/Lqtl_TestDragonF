// Copyright(c) Aurora Devs 2022-2025. All Rights Reserved.

using UnrealBuildTool;

public class AuroraDevs_UGC : ModuleRules
{
    public AuroraDevs_UGC(ReadOnlyTargetRules Target) : base(Target)
    {
        OptimizeCode = CodeOptimization.Never;

        PCHUsage = PCHUsageMode.NoPCHs;
        MinFilesUsingPrecompiledHeaderOverride = 1;
        bUseUnity = false;
        IWYUSupport = IWYUSupport.Full;
        PrecompileForTargets = PrecompileTargetsType.Any;

        PrivateIncludePaths.Add("AuroraDevs_UGC");
        if (Target.Version.MajorVersion >= 5 && Target.Version.MinorVersion >= 5)
        {
            PublicDependencyModuleNames.AddRange(new string[]
            {
                "EngineCameras"
            });
        }

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "EnhancedInput",
            "InputCore" ,
            "LevelSequence",
            "MovieScene",
            "MovieSceneTracks",
            "TemplateSequence"
        });
    }
}
