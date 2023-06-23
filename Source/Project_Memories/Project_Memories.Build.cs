// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Project_Memories : ModuleRules
{
	public Project_Memories(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "UMG", "Engine", "InputCore", "EnhancedInput",
			"MounteaDialogueSystem", "Niagara", "NiagaraCore", "LevelSequence", "MovieScene" });

		PrivateDependencyModuleNames.AddRange(new string[] { "Foliage" });

		// Uncomment if you are using Slate UI
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
