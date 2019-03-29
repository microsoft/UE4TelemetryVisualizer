//--------------------------------------------------------------------------------------
// GameTelemetry.Build.cs
//
// Build rules for Telemetry analytics provider
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------


namespace UnrealBuildTool.Rules
{
	public class GameTelemetry : ModuleRules
	{
        public GameTelemetry( ReadOnlyTargetRules Target ) : base(Target)
		{
            PrivatePCHHeaderFile = "Private/TelemetryPCH.h";

            PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
				});

			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
                    "Http",
                    "Json",
                    "JsonUtilities"
                });
        }
    }
}
