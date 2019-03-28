//--------------------------------------------------------------------------------------
// TelemetryJson.h
//
// Json serialization for telemetry events.
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#pragma once

class GAMETELEMETRY_API FTelemetryJsonSerializer
{
public:
	static void Serialize(const FTelemetryProperties &Container, const TSharedRef<TJsonWriter<>> &Writer);
	static void Serialize(const FTelemetryProperty &Property, const TSharedRef<TJsonWriter<>> &Writer);
};