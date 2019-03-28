//--------------------------------------------------------------------------------------
// TelemetryBuilder.h
//
// Useful wrapper for building telemetry events correctly
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#pragma once

#include "TelemetryInterfaces.h"

class GAMETELEMETRY_API FTelemetryBuilder
{
public:
	FTelemetryBuilder() : Properties() {}

	FTelemetryBuilder(FTelemetryProperties &&Properties) : Properties(Properties) {}
	
	FTelemetryBuilder(const FTelemetryProperties &Properties) : Properties(Properties) {}

	FTelemetryBuilder(FTelemetryPropertiesInitializer InitialProperties) : Properties(InitialProperties)
	{
	}

	FTelemetryBuilder(const FTelemetryBuilder &Builder) : Properties(Builder.Properties) {}

	~FTelemetryBuilder() {}

	template<typename T>
	inline void SetProperty(const FString &Name, const T &Value)
	{
		Properties.Add(Name, Value);
	}

	void SetProperty(const FTelemetryProperty &Property)
	{
		Properties.Add(Property.Key, Property.Value);
	}
	void SetProperty(FTelemetryProperty &&Property)
	{
		Properties.Emplace(MoveTemp(Property.Key), MoveTemp(Property.Value));
	}
	void SetProperties(const FTelemetryProperties &OtherProperties)
	{
		Properties.Append(OtherProperties);
	}

	template<typename T>
	void SetProperty(const FString &Name, T &Value) { SetProperty(FTelemetryProperty(Name, Value)); }

	template<typename T>
	void SetProperty(const FString &&Name, T &&Value) { SetProperty(FTelemetryProperty(Name, Value)); }

	void SetProperties(FTelemetryPropertiesInitializer InitialProperties) { SetProperties(FTelemetryProperties(InitialProperties)); }

	void GetPropertiesFromProvider(ITelemetryProvider &Provider)
	{
		Provider.ProvideTelemetry(*this);
	}

	const FTelemetryProperties &GetProperties() const { return Properties; }

	const FVariant & operator[](const FString &Name) const { return Properties[Name]; }

private:
	FTelemetryProperties Properties;
};