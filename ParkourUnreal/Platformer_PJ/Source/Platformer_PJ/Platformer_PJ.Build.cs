// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Platformer_PJ : ModuleRules
{
	public Platformer_PJ(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
