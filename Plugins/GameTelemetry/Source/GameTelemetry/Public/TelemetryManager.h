//--------------------------------------------------------------------------------------
// TelemetryManager.h
//
// Publishing telemetry functionality
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#pragma once

#include "CoreMinimal.h"
#include "TelemetryInterfaces.h"
#include "TelemetryBuilder.h"

// Storage class for telemetry configuration
class GAMETELEMETRY_API FTelemetryConfiguration
{
public:
	// URL of the telemetry ingestion server
	FString IngestionUrl;

	// The interval between flushes
	double SendInterval = 10.0;

	// Number of events that can be pending before events are lost
	int32 PendingBufferSize = 128;

public:
	static const FString &GetIniFileName() { return IniFileName; }
	static const FString &GetIniSectionName() { return IniSectionName; }

	static bool GetString(const TCHAR *Key, FString &Value) { return GConfig->GetString(*IniSectionName, Key, Value, IniFileName); }
	static bool GetDouble(const TCHAR *Key, double &Value) { return GConfig->GetDouble(*IniSectionName, Key, Value, IniFileName); }
	static bool GetInt(const TCHAR *Key, int32 &Value) { return GConfig->GetInt(*IniSectionName, Key, Value, IniFileName); }

private:
	static FString IniFileName;
	static FString IniSectionName;
};


class GAMETELEMETRY_API FTelemetryManager
{
	// Singleton for common properties
public:
	// Intialization of the singleton fromm the provided configuration
	static void Initialize(const FTelemetryConfiguration &Config);

	// Helper function to read configuration values from the ini
	static FTelemetryConfiguration GetConfigFromIni();

	// Set a new client id - Usually set to the platform client id
	// If setting this again after initialization, consider flushing existing buffered telemetry or they may be incorrectly associated with the new id
	void SetClientId(const FString &InClientId);

	// Set a new session id - Usually set when the game would like to differentiate between play sessions (such as changing users)
	// If setting this more than once, consider flushing existing buffered telemetry or they may be incorrectly associated with the new id
	void SetSessionId(const FString &InSessionId);

	// Set a common property which will be included in all telemetry sent
	template<typename T>
	void SetCommonProperty(const FString &Name, const T &Value)
	{
		Instance->CommonProperties.SetProperty(Name, Value);
	}

	// Set a common property which will be included in all telemetry sent
	void SetCommonProperty(const FTelemetryProperty &Property)
	{
		Instance->CommonProperties.SetProperty(Property);
	}

	// Set a common property which will be included in all telemetry sent
	void SetCommonProperty(FTelemetryProperty &&Property)
	{
		Instance->CommonProperties.SetProperty(Property);
	}

	// Set a common property which will be included in all telemetry sent
	const FTelemetryProperties &GetCommonProperties()
	{
		return Instance->CommonProperties.GetProperties();
	}

	// Singleton reference
	static FTelemetryManager &Get() { check(Instance.IsValid()) return *Instance; }

	// Get the currently set client id
	static FString GetClientId() { return Instance->CommonProperties["client_id"].GetValue<FString>(); }

	// Get the currently set session id
	static FString GetSessionId() { return Instance->CommonProperties["session_id"].GetValue<FString>(); }

	/**
		Records an event and places it in the buffer to be sent
		@param Name: Event name
		@param Category: Category
		@param Version: Semantic version of this event
		@param PropertiesBuilder: Properties to set in this event
	*/
	void Record(const FString &Name, const FString &Category, const FString &Version, FTelemetryBuilder &&PropertiesBuilder);


	// Flushes any pending telemetry and shuts down the singleton
	void Shutdown();

	~FTelemetryManager() {};

private:
	FTelemetryManager(FTelemetryConfiguration Configuration) : Configuration(Configuration), CommonProperties() {}

private:
	// Singleton instance
	static TUniquePtr<FTelemetryManager> Instance;

private:
	// Common properties which are sent with all events
	FTelemetryBuilder CommonProperties;
	FTelemetryConfiguration Configuration;
};