// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class LedgeDetection : ModuleRules
{
	public LedgeDetection(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
