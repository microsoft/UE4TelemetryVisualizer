//--------------------------------------------------------------------------------------
// TelemetryVisualizerModule.cpp
//
// Provides module interface for editor visualization
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#include "TelemetryVisualizerModule.h"
#include "TelemetryVisualizerUI.h"

DEFINE_LOG_CATEGORY(LogTelemetryVisualizer);
IMPLEMENT_MODULE(FTelemetryVisualizerModule, TelemetryVisualizerModule);

void FTelemetryVisualizerModule::StartupModule()
{
	EditorUI = TUniquePtr<FTelemetryVisualizerUI>(new FTelemetryVisualizerUI);
	EditorUI->Initialize();
}

void FTelemetryVisualizerModule::ShutdownModule()
{
	EditorUI->Shutdown();
}