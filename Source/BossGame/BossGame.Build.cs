using UnrealBuildTool;

public class BossGame : ModuleRules
{
	public BossGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "AIModule", "NavigationSystem",
			"GameplayAbilities", "GameplayTags", "GameplayTasks", "MotionWarping" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

        PublicIncludePaths.Add(ModuleDirectory);
    }
}
