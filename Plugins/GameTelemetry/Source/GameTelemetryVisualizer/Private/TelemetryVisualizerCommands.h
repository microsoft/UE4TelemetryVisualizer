//--------------------------------------------------------------------------------------
// TelemetryVisualizerCommands.h
//
// Telemetry editor plugin commands.  These provide the UI options when running Telemetry in editor
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#pragma once

#if WITH_EDITOR
#include "Framework/Commands/Commands.h"
#include "Editor/EditorStyle/Public/EditorStyleSet.h"

class TelemetryVisualizerCommands : public TCommands<TelemetryVisualizerCommands>
{
public:

	TelemetryVisualizerCommands()
		: TCommands<TelemetryVisualizerCommands>(TEXT("TelemetryVisualizer"), NSLOCTEXT("Contexts", "TelemetryVisualizer", "Telemetry Plugin"), NAME_None, FEditorStyle::GetStyleSetName())
	{
	}

	virtual void RegisterCommands() override;

	TSharedPtr< FUICommandInfo > m_displayDataTab;
	TSharedPtr< FUICommandInfo > m_displayVizTab;
	TSharedPtr< FUICommandInfo > m_displayAll;
};
#endif