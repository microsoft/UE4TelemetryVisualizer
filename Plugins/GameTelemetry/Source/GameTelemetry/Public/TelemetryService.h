// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.
//--------------------------------------------------------------------------------------
// TelemetryService.h
//
// Utility to help create client requests to the telemetry service.
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#pragma once
#include "CoreMinimal.h"
#include "Http.h"
#include "TelemetryManager.h"

class GAMETELEMETRY_API FTelemetryService
{
public:
	// The authentication, if used, to authenticate with the Azure Functions


	static TSharedRef<IHttpRequest> CreateServiceRequest()
	{
		if (!IsInitialized)
		{
			UseKey = FTelemetryConfiguration::GetString(TEXT("AuthenticationKey"), AuthenticationKey);
			IsInitialized = true;
		}

		auto request = FHttpModule::Get().CreateRequest();

		// @todo: Add any other common http request initialization code here

		if (UseKey)
		{
			// https://docs.microsoft.com/en-us/azure/azure-functions/functions-bindings-http-webhook#api-key-authorization
			request->SetHeader(TEXT("x-functions-key"), AuthenticationKey);
		}
		return request;
	}

private:
	static FString AuthenticationKey;
	static bool UseKey;
	static bool IsInitialized;
};