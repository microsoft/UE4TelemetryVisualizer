//--------------------------------------------------------------------------------------
// TelemetryVisualizerPCH.h
//
// PCH for editor elements
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "TelemetryVisualizerCommands.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Colors/SColorPicker.h"
#include "Widgets/Input/SSlider.h"
#include "Widgets/Docking/SDockableTab.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Brushes/SlateColorBrush.h"
#include "LevelEditor.h"
#include "Editor.h"
#include "DrawDebugHelpers.h"
#include "Containers/Ticker.h"
#include "TelemetryEvent.h"

static const FName TelemetryDataIndirectTabName("TelemetryDataTab");
static const FName TelemetryVizIndirectTabName("TelemetryVizTab");

