// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class EX3_1 : ModuleRules
{
	public EX3_1(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate",
			"SlateCore"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"EX3_1",
			"EX3_1/Variant_Platforming",
			"EX3_1/Variant_Platforming/Animation",
			"EX3_1/Variant_Combat",
			"EX3_1/Variant_Combat/AI",
			"EX3_1/Variant_Combat/Animation",
			"EX3_1/Variant_Combat/Gameplay",
			"EX3_1/Variant_Combat/Interfaces",
			"EX3_1/Variant_Combat/UI",
			"EX3_1/Variant_SideScrolling",
			"EX3_1/Variant_SideScrolling/AI",
			"EX3_1/Variant_SideScrolling/Gameplay",
			"EX3_1/Variant_SideScrolling/Interfaces",
			"EX3_1/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
