//--------------------------------------------------------------------------------------
// TelemetryInterfaces.h
//
// Common types and interfaces used throughout the Game Telemetry plugin.
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#pragma once

// A single telemetry property 
typedef TPair<FString, FVariant> FTelemetryProperty;
// A collection of telemetry properties
typedef TMap<FString, FVariant> FTelemetryProperties;
// Initializer for telemetry properties used by many helper functions
typedef std::initializer_list<TPairInitializer<const FString &, const FVariant &>> FTelemetryPropertiesInitializer;

// An interface for allowing implementers to provide telemetry properties to a builder
class GAMETELEMETRY_API ITelemetryProvider
{
public:
	virtual ~ITelemetryProvider() {}

	// Adds properties to a telemetry builder
	virtual void ProvideTelemetry(class FTelemetryBuilder &Builder) = 0;
};