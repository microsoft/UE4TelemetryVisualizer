//--------------------------------------------------------------------------------------
// Telemetry.cpp
//
// Implementation for common telemetry functions.
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#include "Telemetry.h"
#include "TelemetryPCH.h"
#include "TelemetryService.h"
#include "TelemetryJson.h"

#include "TelemetryService.h"

DEFINE_LOG_CATEGORY(LogTelemetry);

FString FTelemetry::DumpJson(FTelemetryProperties Properties)
{
	FString Result;

	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Result);

	Writer->WriteObjectStart();
	FTelemetryJsonSerializer::Serialize(Properties, Writer);
	Writer->WriteObjectEnd();

	Writer->Close();

	return Result;
}


