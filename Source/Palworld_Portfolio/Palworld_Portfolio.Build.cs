// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Palworld_Portfolio : ModuleRules
{
	public Palworld_Portfolio(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "AIModule", "UMG" });
        PrivateDependencyModuleNames.AddRange(new string[] { "AssetRegistry" });
    }
}
