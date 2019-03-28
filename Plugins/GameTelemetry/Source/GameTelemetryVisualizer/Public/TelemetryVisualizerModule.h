//--------------------------------------------------------------------------------------
// TelemetryVisualizerModule.h
//
// Provides module interface for editor visualization
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTelemetryVisualizer, Log, All);

class ITelemetryEngine;
class FTelemetryVisualizerUI;

class FTelemetryVisualizerModule :
	public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	TUniquePtr<FTelemetryVisualizerUI> EditorUI;
};

