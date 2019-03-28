//--------------------------------------------------------------------------------------
// GameTelemetryVisualizer.Build.cs
//
// Build rules for Telemetry editor
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

using UnrealBuildTool;

public class GameTelemetryVisualizer : ModuleRules
{
    public GameTelemetryVisualizer(ReadOnlyTargetRules Target) : base(Target)
	{
        PrivatePCHHeaderFile = "Private/TelemetryVisualizerPCH.h";

		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"CoreUObject",
                "GameTelemetry",
                "AnalyticsVisualEditing",
                "AppFramework",
                "Engine",
				"Projects",
                "Slate",
                "SlateCore",
				"LevelEditor",
				"EditorStyle",
				"UnrealEd",
                "InputCore",
                "Http",
                "Json",
                "JsonUtilities"
            }
            );

		PrivateIncludePathModuleNames.AddRange(
			new string[]
            {
				"Settings"
			}
		);
	}
}
