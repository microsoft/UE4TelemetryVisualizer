// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.
//--------------------------------------------------------------------------------------
// TelemetrySettings.h
//
// Telemetry editor settings.  These provide the UI for editor preferences
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "AnalyticsSettings.h"
#include "TelemetrySettings.generated.h"

USTRUCT()
struct FTelemetryConfigSetting
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category=Telemetry, meta=(ConfigRestartRequired=true))
	FString QueryUrl;

	UPROPERTY(EditAnywhere, Category=Telemetry, meta=(ConfigRestartRequired=true))
	FString IngestUrl;

	UPROPERTY(EditAnywhere, Category=Telemetry, meta=(ConfigRestartRequired=true))
	int32 SendInterval;

	UPROPERTY(EditAnywhere, Category=Telemetry, meta=(ConfigRestartRequired=true))
	int32 MaxBufferSize;

	UPROPERTY(EditAnywhere, Category = Telemetry, meta = (ConfigRestartRequired = true))
	int32 QueryTakeLimit;

	UPROPERTY(EditAnywhere, Category = Telemetry, meta = (ConfigRestartRequired = true))
	FString AuthenticationKey;
};

UCLASS()
class UTelemetrySettings
	: public UAnalyticsSettingsBase
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, Category=Telemetry, meta=(ConfigRestartRequired=true))
	FTelemetryConfigSetting Release;

	UPROPERTY(EditAnywhere, Category=Telemetry, meta=(ConfigRestartRequired=true))
	FTelemetryConfigSetting Debug;

	UPROPERTY(EditAnywhere, Category=Telemetry, meta=(ConfigRestartRequired=true))
	FTelemetryConfigSetting Test;

	UPROPERTY(EditAnywhere, Category=Telemetry, meta=(ConfigRestartRequired=true))
	FTelemetryConfigSetting Development;

// UTelemetrySettingsBase interface
protected:
	/**
	 * Provides a mechanism to read the section based information into this UObject's properties
	 */
	virtual void ReadConfigSettings();
	/**
	 * Provides a mechanism to save this object's properties to the section based ini values
	 */
	virtual void WriteConfigSettings();

private:
	void ReadConfigStruct(const FString& Section, FTelemetryConfigSetting& Dest, FTelemetryConfigSetting* Default = nullptr);
	void WriteConfigStruct(const FString& Section, const FTelemetryConfigSetting& Source);
};
