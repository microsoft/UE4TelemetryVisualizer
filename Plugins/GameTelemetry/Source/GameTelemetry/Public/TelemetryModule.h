//--------------------------------------------------------------------------------------
// TelemetryModule.h
//
// Component for linking the Telemetry engine module with Unreal's Telemetry service
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
/**
 * The public interface to this module
 */
class FTelemetryModule : public IModuleInterface
{
	//--------------------------------------------------------------------------
	// Module functionality
	//--------------------------------------------------------------------------
public:
	/**
	 * Singleton-like access to this module's interface.  This is just for convenience!
	 * Beware of calling this during the shutdown phase, though.  Your module might have been unloaded already.
	 *
	 * @return Returns singleton instance, loading the module on demand if needed
	 */
	static inline FTelemetryModule& Get()
	{
		return FModuleManager::LoadModuleChecked< FTelemetryModule >( "Telemetry" );
	}

	~FTelemetryModule() {}
	
private:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

