using UnrealBuildTool;

public class Tides_Of_Nusantara : ModuleRules
{
	public Tides_Of_Nusantara(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
			"AIModule",
			"Water",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"MassEntity",
			"MassCrowd",
			"MassActors",
		"UMG",
		"Slate",
		"SlateCore",
		"HTTP",
		"Json"
		});
	}
}
