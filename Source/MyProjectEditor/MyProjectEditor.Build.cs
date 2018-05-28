// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class MyProjectEditor : ModuleRules
{
	public MyProjectEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicIncludePaths.AddRange(new string[] {"MyProjectEditor/Public"});
		PrivateIncludePaths.AddRange(new string[] {"MyProjectEditor/Private"});
		PublicDependencyModuleNames.AddRange(new string[]{});
		PrivateDependencyModuleNames.AddRange(new string[]{
			"MyProject",
			"Core",
            "Engine",
            "CoreUObject",
            "UnrealEd",
            "BlueprintGraph",
            "GraphEditor",
            "PropertyEditor",
            "SlateCore",
            "Slate",
            "EditorStyle",
            "Kismet",
            "KismetCompiler",
		    "GameplayAbilities",
			"GameplayTags"
		});
	}
}

