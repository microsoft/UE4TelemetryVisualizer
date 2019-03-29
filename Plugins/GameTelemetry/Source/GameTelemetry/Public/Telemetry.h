//--------------------------------------------------------------------------------------
// Telemetry.h
//
// Telemetry definition common functions.
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "Misc/Variant.h"

#include "TelemetryInterfaces.h"
#include "TelemetryBuilder.h"
#include "TelemetryManager.h"

GAMETELEMETRY_API DECLARE_LOG_CATEGORY_EXTERN(LogTelemetry, Log, All);

// Format a semantic version string given the provided values
FORCEINLINE FString FormatVersion(int32 Major, int32 Minor, int32 Patch) { return FString::Printf(TEXT("%d.%d.%d"), Major, Minor, Patch); }


// Common usage library for telemetry
class GAMETELEMETRY_API FTelemetry
{
public:
	/**
		Creates an empty telemetry builder
	*/
	FORCEINLINE static FTelemetryBuilder Create() { return FTelemetryBuilder(); }

	/**
		Creates a telemetry builder
		@param Properties: Initial properties to set in this event
	*/
	FORCEINLINE static FTelemetryBuilder Create(FTelemetryPropertiesInitializer InitialProperties = {})
	{
		FTelemetryBuilder Evt(InitialProperties);
		return Evt;
	}

	/**
		Creates a telemetry builder
		@param Properties: Initial properties to set in this event
	*/
	FORCEINLINE static FTelemetryBuilder Create(FTelemetryProperties &&Properties)
	{
		FTelemetryBuilder Evt(MoveTemp(Properties));
		return Evt;
	}

	/**
		Records an event and places it in the buffer to be sent
		@param Name: Event name
		@param Category: Category
		@param Version: Semantic version of this event
		@param Properties: Properties to set in this event
	*/
	FORCEINLINE static void Record(const FString &Name, const FString &Category, const FString &Version, const FTelemetryBuilder &Properties)
	{
		FTelemetryManager::Get().Record(Name, Category, Version, FTelemetryBuilder(Properties));
	}

	/**
		Records an event and places it in the buffer to be sent
		@param Name: Event name
		@param Category: Category
		@param Version: Semantic version of this event
		@param Properties: Properties to set in this event
	*/
	FORCEINLINE static void Record(const FString &Name, const FString &Category, const FString &Version, const FTelemetryProperties &Properties)
	{ 
		FTelemetryManager::Get().Record(Name, Category, Version, FTelemetryBuilder(Properties));
	}

	/**
		Records an event and places it in the buffer to be sent
		@param Name: Event name
		@param Category: Category
		@param Version: Semantic version of this event
		@param InitialProperties: Initializer list of properties to set in this event
	*/
	FORCEINLINE static void Record(const FString &Name, const FString &Category, const FString &Version, FTelemetryPropertiesInitializer InitialProperties = {})
	{
		FTelemetryManager::Get().Record(Name, Category, Version, Create(InitialProperties));
	}

// Helper functions for consistently formatting special properties
public:
	// Name of the event
	FORCEINLINE static FTelemetryProperty EventName(const FString &Name) { return FTelemetryProperty(TEXT("name"), Name); }

	// Category for an event - Useful for grouping events of a similary type
	FORCEINLINE static FTelemetryProperty Category(const FString &Value) { return FTelemetryProperty(TEXT("cat"), Value); }

	// Position vector for an entity
	FORCEINLINE static FTelemetryProperty Position(const FVector &Value) { return FTelemetryProperty(TEXT("pos"), Value); }

	// Orientation unit vector for an entity
	FORCEINLINE static FTelemetryProperty Orientation(const FVector &Vec) { return FTelemetryProperty(TEXT("dir"), Vec); }

	// The name of the value the visualizer will use
	FORCEINLINE static FTelemetryProperty DisplayValue(const FString &Value) { return FTelemetryProperty("disp_val", Value); }

	// Timestamp of the event using the client's clock by default
	FORCEINLINE static FTelemetryProperty ClientTimestamp(const FDateTime &Value = FDateTime::UtcNow()) { return FTelemetryProperty("client_ts", Value); }

	// Unique client id for the device playing the game
	// Typically set in the Common Properties
	FORCEINLINE static FTelemetryProperty ClientId(const FString &Value) { return FTelemetryProperty(TEXT("client_id"), Value); }

	// Unique user id for the user playing the game
	// Typically set in the Common Properties
	FORCEINLINE static FTelemetryProperty UserId(const FString &Value) { return FTelemetryProperty(TEXT("user_id"), Value); }

	// Unique session id for the current play session
	FORCEINLINE static FTelemetryProperty SessionId(const FString &Value) { return FTelemetryProperty(TEXT("session_id"), Value); }

	// Semantic version of the telemetry event
	// Use this to help data pipelines understand how to process the event after ingestion
	FORCEINLINE static FTelemetryProperty Version(const FString &Value) { return FTelemetryProperty(TEXT("tver"), Value); }

	// Semantic version of a component of the telemetry - Typically used by Telemetry Providers
	// Use this to help data pipelines understand how to process the event after ingestion
	FORCEINLINE static FTelemetryProperty Version(const FString &SubEntity, const FString &Value) { return FTelemetryProperty(TEXT("tver_") + SubEntity, Value); }

	// A value that represents a percentage float between 0 and 100
	FORCEINLINE static FTelemetryProperty Percentage(const FString &SubEntity, const float &Value)
	{
		check(Value >= 0.f && Value <= 100.f);
		return FTelemetryProperty(TEXT("pct_") + SubEntity, Value);
	}

	// A float value
	FORCEINLINE static FTelemetryProperty Value(const FString &SubEntity, const float &Value) { return FTelemetryProperty(TEXT("val_") + SubEntity, Value);	}

	// Generic telemetry property maker
	template<typename T>
	FORCEINLINE static FTelemetryProperty Prop(const FString &Name, const T &Value) { return FTelemetryProperty(Name, Value); }


public:
	// Debug utility function for outputting telemetry to a string for printing
	static FString DumpJson(FTelemetryProperties Properties);
};
