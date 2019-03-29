//--------------------------------------------------------------------------------------
// TelemetrySettings.cpp
//
// Telemetry editor settings.  These load the Telemetry connection settings from the ini
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#include "TelemetrySettings.h"
#include "TelemetryVisualizerPCH.h"

#define LOCTEXT_NAMESPACE "Telemetry"

UTelemetrySettings::UTelemetrySettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SettingsDisplayName = LOCTEXT("SettingsDisplayName", "Telemetry");
	SettingsTooltip = LOCTEXT("SettingsTooltip", "Telemetry configuration settings");
}

FTelemetryConfigSetting GetDefaultSettings()
{
	FTelemetryConfigSetting Default;
	Default.IngestUrl = "https://<yourazurefunctionapp>.azurewebsites.net/api/ingest";
	Default.QueryUrl = "https://<yourazurefunctionapp>.azurewebsites.net/api/query";
	Default.MaxBufferSize = 128;
	Default.SendInterval = 60;
	Default.QueryTakeLimit = 10000;
	Default.AuthenticationKey = "<Enter your Azure Functions authentication key here>";

	return Default;
}

void UTelemetrySettings::ReadConfigSettings()
{
	FTelemetryConfigSetting DefaultSettings = GetDefaultSettings();

	ReadConfigStruct(GetDevelopmentIniSection(), Development, &DefaultSettings);
	ReadConfigStruct(GetReleaseIniSection(), Release, &Development);
	ReadConfigStruct(GetTestIniSection(), Test, &Development);
	ReadConfigStruct(GetDebugIniSection(), Debug, &Development);
}

void UTelemetrySettings::WriteConfigSettings()
{
	WriteConfigStruct(GetReleaseIniSection(), Release);
	WriteConfigStruct(GetTestIniSection(), Test);
	WriteConfigStruct(GetDebugIniSection(), Debug);
	WriteConfigStruct(GetDevelopmentIniSection(), Development);
}

FString GetConfigValueFromIni(const FString &KeyName, bool IsRequired)
{
	const FString IniName = FString::Printf(TEXT("%sGameTelemetry.ini"), *FPaths::SourceConfigDir());
	const FString SectionName = "GameTelemetry";

	FString Result;
	bool Success = GConfig->GetString(*SectionName, *KeyName, Result, IniName);

	// Don't error if the configuration file isn't created yet.  Let the editor help make it!
	if (IsRequired && FPaths::FileExists(IniName))
	{
		check(Success);
	}
	return Result;
}

void SetConfigValueFromIni(const FString &KeyName, const FString &Value)
{
	const FString IniName = FString::Printf(TEXT("%sGameTelemetry.ini"), *FPaths::SourceConfigDir());
	const FString SectionName = "GameTelemetry";

	GConfig->SetString(*SectionName, *KeyName, *Value, IniName);
}

void UTelemetrySettings::ReadConfigStruct(const FString& Section, FTelemetryConfigSetting& Dest, FTelemetryConfigSetting* Default)
{
	const FString QueryUrl = GetConfigValueFromIni(TEXT("QueryUrl"), true);
	if (QueryUrl.Len() == 0 && Default != nullptr)
	{
		Dest.QueryUrl = Default->QueryUrl;
	}
	else
	{
		Dest.QueryUrl = QueryUrl;
	}
	const FString IngestUrl = GetConfigValueFromIni(TEXT("IngestUrl"), true);
	if (IngestUrl.Len() == 0 && Default != nullptr)
	{
		Dest.IngestUrl = Default->IngestUrl;
	}
	else
	{
		Dest.IngestUrl = IngestUrl;
	}
	const FString SendInterval = GetConfigValueFromIni(TEXT("SendInterval"), false);
	if (SendInterval.Len() == 0 && Default != nullptr)
	{
		Dest.SendInterval = Default->SendInterval;
	}
	else
	{
		Dest.SendInterval = FCString::Atoi(*SendInterval);
	}
	const FString MaxBufferSize = GetConfigValueFromIni(TEXT("MaxBufferSize"), false);
	if (MaxBufferSize.Len() == 0 && Default != nullptr)
	{
		Dest.MaxBufferSize = Default->MaxBufferSize;
	}
	else
	{
		Dest.MaxBufferSize = FCString::Atoi(*MaxBufferSize);
	}
	const FString QueryTakeLimit = GetConfigValueFromIni(TEXT("QueryTakeLimit"), false);
	if (QueryTakeLimit.Len() == 0 && Default != nullptr)
	{
		Dest.QueryTakeLimit = Default->QueryTakeLimit;
	}
	else
	{
		Dest.QueryTakeLimit = FCString::Atoi(*QueryTakeLimit);
	}
	const FString AuthenticationKey = GetConfigValueFromIni(TEXT("AuthenticationKey"), false);
	if (AuthenticationKey.Len() == 0 && Default != nullptr)
	{
		Dest.AuthenticationKey = Default->AuthenticationKey;
	}
	else
	{
		Dest.AuthenticationKey = AuthenticationKey;
	}
}

void UTelemetrySettings::WriteConfigStruct(const FString& Section, const FTelemetryConfigSetting& Source)
{
	SetConfigValueFromIni(TEXT("QueryUrl"), Source.QueryUrl);
	SetConfigValueFromIni(TEXT("IngestUrl"), Source.IngestUrl);
	SetConfigValueFromIni(TEXT("SendInterval"), FString::Printf(TEXT("%d"), Source.SendInterval));
	SetConfigValueFromIni(TEXT("MaxBufferSize"), FString::Printf(TEXT("%d"), Source.MaxBufferSize));
	SetConfigValueFromIni(TEXT("QueryTakeLimit"), FString::Printf(TEXT("%d"), Source.QueryTakeLimit));
	SetConfigValueFromIni(TEXT("AuthenticationKey"), Source.AuthenticationKey);
}

#undef LOCTEXT_NAMESPACE
