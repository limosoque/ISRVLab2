// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ISRV : ModuleRules
{
	public ISRV(ReadOnlyTargetRules Target) : base(Target)
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
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"ISRV",
			"ISRV/Variant_Platforming",
			"ISRV/Variant_Platforming/Animation",
			"ISRV/Variant_Combat",
			"ISRV/Variant_Combat/AI",
			"ISRV/Variant_Combat/Animation",
			"ISRV/Variant_Combat/Gameplay",
			"ISRV/Variant_Combat/Interfaces",
			"ISRV/Variant_Combat/UI",
			"ISRV/Variant_SideScrolling",
			"ISRV/Variant_SideScrolling/AI",
			"ISRV/Variant_SideScrolling/Gameplay",
			"ISRV/Variant_SideScrolling/Interfaces",
			"ISRV/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
