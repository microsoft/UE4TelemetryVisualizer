//--------------------------------------------------------------------------------------
// TelemetryVisualizerCommands.cpp
//
// Telemetry editor plugin commands.  These provide the UI options when running Telemetry in editor
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#include "TelemetryVisualizerCommands.h"
#include "TelemetryVisualizerPCH.h"
#if WITH_EDITOR

void TelemetryVisualizerCommands::RegisterCommands()
{
#define LOCTEXT_NAMESPACE "TelemetryVisualizerCommands"
	UI_COMMAND(m_displayDataTab, "Data Viewer", "Data Viewer", EUserInterfaceActionType::Check, FInputGesture());
	UI_COMMAND(m_displayVizTab, "Visualization Tools", "Vizualization Tools", EUserInterfaceActionType::Check, FInputGesture());
	UI_COMMAND(m_displayAll, "Open All", "Open All", EUserInterfaceActionType::Check, FInputGesture());
#undef LOCTEXT_NAMESPACE
}

#endif
