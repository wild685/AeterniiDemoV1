// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AeterniiDemoV1 : ModuleRules
{
	public AeterniiDemoV1(ReadOnlyTargetRules Target) : base(Target)
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
			"GameplayTags",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"AeterniiDemoV1",
			"AeterniiDemoV1/Variant_Platforming",
			"AeterniiDemoV1/Variant_Platforming/Animation",
			"AeterniiDemoV1/Variant_Combat",
			"AeterniiDemoV1/Variant_Combat/AI",
			"AeterniiDemoV1/Variant_Combat/Animation",
			"AeterniiDemoV1/Variant_Combat/Gameplay",
			"AeterniiDemoV1/Variant_Combat/Interfaces",
			"AeterniiDemoV1/Variant_Combat/UI",
			"AeterniiDemoV1/Variant_SideScrolling",
			"AeterniiDemoV1/Variant_SideScrolling/AI",
			"AeterniiDemoV1/Variant_SideScrolling/Gameplay",
			"AeterniiDemoV1/Variant_SideScrolling/Interfaces",
			"AeterniiDemoV1/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
