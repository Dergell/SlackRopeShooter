// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SlackRopeShooter : ModuleRules
{
	public SlackRopeShooter(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput" });
		
		PrivateDependencyModuleNames.AddRange(new string[] { "CableComponent" });
	}
}
