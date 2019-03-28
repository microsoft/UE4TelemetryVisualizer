//--------------------------------------------------------------------------------------
// TelemetryModule.cpp
//
// Component for linking the Telemetry engine module with Unreal's Telemetry service
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#include "TelemetryModule.h"
#include "TelemetryPCH.h"
#include "Modules/ModuleManager.h"
#include "HAL/PlatformProcess.h"

IMPLEMENT_MODULE(FTelemetryModule, Telemetry)

void FTelemetryModule::StartupModule()
{
}

void FTelemetryModule::ShutdownModule()
{
	
}
