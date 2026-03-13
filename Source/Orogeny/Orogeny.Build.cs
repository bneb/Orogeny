// Copyright Orogeny. All Rights Reserved.

using UnrealBuildTool;

public class Orogeny : ModuleRules
{
	public Orogeny(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
			"Niagara"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Slate",
			"SlateCore",
			"UMG",
			"AnimGraphRuntime",
			"ControlRig"
		});

		// Include the Tests subdirectory for automation tests
		PublicIncludePaths.AddRange(new string[]
		{
			"Orogeny/Tests"
		});
	}
}
