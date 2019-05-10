// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.
//--------------------------------------------------------------------------------------
// TelemetryManager.h
//
// Publishing telemetry functionality
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#include "TelemetryManager.h"
#include "TelemetryPCH.h"
#include "TelemetryService.h"
#include "Telemetry.h"

FString FTelemetryService::AuthenticationKey;
bool FTelemetryService::IsInitialized = false;
bool FTelemetryService::UseKey = false;


TUniquePtr<FTelemetryManager> FTelemetryManager::Instance;
bool hasInit = false;

TAtomic<uint32> Sequence;

class FTelemetryBatchPayload
{
public:
    FTelemetryBatchPayload(const FTelemetryProperties &Common) :
        Payload(),
        IsFinalized(false),
        Writer(TJsonWriterFactory<>::Create(&Payload))
    {
        Writer->WriteObjectStart(); // Outer most object
        Writer->WriteObjectStart("header"); // header portion
        FTelemetryJsonSerializer::Serialize(Common, Writer);
        Writer->WriteObjectEnd();
        Writer->WriteArrayStart("events"); // Events array
    }

    void AddTelemetry(FTelemetryProperties Event)
    {
        check(!IsFinalized);

        Writer->WriteObjectStart();
        FTelemetryJsonSerializer::Serialize(Event, Writer);
        Writer->WriteObjectEnd();
    }

    const FString & Finalize()
    {
        check(!IsFinalized); // Shouldn't call finalize more than once

        if (!IsFinalized) // Nothing really bad happens though...
        {
            Writer->WriteArrayEnd();
            Writer->WriteObjectEnd();
            Writer->Close();
            IsFinalized = true;
        }

        check(Payload.IsEmpty() == false);
        return Payload;
    }

    const FString &GetPayload() { return Payload; }

private:
    FString Payload;
    TSharedRef<TJsonWriter<>> Writer;
    bool IsFinalized;
};

class FTelemetryWorker : public FRunnable
{
public:

    FTelemetryWorker(FString IngestUrl, double SendInterval = 10.0, int PendingBufferSize = 127) :
        IngestUrl(IngestUrl),
        SendInterval(SendInterval),
        ShouldRun(true),
        IsComplete(false),
        Pending(PendingBufferSize)
    {

        Thread = TUniquePtr<FRunnableThread>(FRunnableThread::Create(this, TEXT("TelemetryUploadThread"), 0, EThreadPriority::TPri_BelowNormal));
    }

    virtual ~FTelemetryWorker()
    {
        Sync.Release();
        Thread.Release();
    }

    virtual bool Init() override
    {
        Sync = TUniquePtr<FEvent>(FGenericPlatformProcess::GetSynchEventFromPool());
        return true;
    }

    virtual uint32 Run() override
    {
        while (ShouldRun)
        {
            Sync->Wait(FTimespan::FromSeconds(SendInterval));

            // Flush here

            if (Pending.Count() > 0)
            {
                SendTelemetry(FTelemetryManager::Get().GetCommonProperties());
            }
        }

        return 0;
    }

    virtual void Stop() override
    {
        ShouldRun = false;

        TriggerFlush();

        IsComplete = true;
    }

    virtual void Exit() override
    {
        if (!IsComplete)
        {
            Stop();
            Thread->WaitForCompletion();
        }
    }

    void TriggerFlush()
    {
        if (Sync)
        {
            Sync->Trigger();
        }
    }

    void Enqueue(TSharedPtr<FTelemetryBuilder> Properties)
    {
        // Circular Queue/Buffer want SPSC model, so limit to the most likely thread telemetry will be generated on
        if (!IsInGameThread())
        {
            AsyncTask(ENamedThreads::GameThread, [this, Properties]() {
                Pending.Enqueue(Properties);
            });
        }
        else
        {
            Pending.Enqueue(Properties);
        }
    }

    bool CompressPayload(const FString &Payload, TArray<uint8> &Compressed)
    {
        FTCHARToUTF8 PayloadUTF8(*Payload);

        const void *UncompressedPtr = PayloadUTF8.Get();
        const int32 UncompressedSize = PayloadUTF8.Length() + 1;
        int32 CompressedSize = 0;

        Compressed.AddUninitialized(UncompressedSize * 4 / 3);
        bool CompressSuccessful = FCompression::CompressMemory((ECompressionFlags)(COMPRESS_GZIP | COMPRESS_BiasMemory), Compressed.GetData(), CompressedSize, UncompressedPtr, UncompressedSize);
        if (CompressSuccessful)
        {
            Compressed.SetNum(CompressedSize, false);
        }

        return CompressSuccessful;
    }

    void SendTelemetry(const FTelemetryProperties &CommonProperties)
    {
        auto request = FTelemetryService::CreateServiceRequest();

        FTelemetryBatchPayload BatchPayload(CommonProperties);

        TSharedPtr<FTelemetryBuilder> Event;
        while (Pending.Dequeue(Event))
        {
            BatchPayload.AddTelemetry(Event->GetProperties());
        }

        request->SetURL(IngestUrl);
        request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
        request->SetHeader(TEXT("x-ms-payload-type"), TEXT("batch"));
        request->SetVerb(TEXT("POST"));

        const FString &Payload = BatchPayload.Finalize();
        TArray<uint8> Compressed;

        if (CompressPayload(Payload, Compressed))
        {
            request->SetContent(Compressed);
            request->SetHeader(TEXT("Content-Encoding"), TEXT("gzip"));
        }
        else
        {
            request->SetContentAsString(Payload);
        }

        request->OnProcessRequestComplete().BindLambda([](FHttpRequestPtr req, FHttpResponsePtr resp, bool successful)
        {
            UE_LOG(LogTelemetry, Display, TEXT("Http telemetry ingestion attempt %s."), successful ? TEXT("succeeded") : TEXT("failed"));
            if (resp.IsValid())
            {
                UE_LOG(LogTelemetry, Display, TEXT("Http Ingestion Response Status: %d"), resp->GetResponseCode());
                if (resp->GetResponseCode() >= 400)
                {
                    UE_LOG(LogTelemetry, Error, TEXT("Telemetry Error Information: %s"), *resp->GetContentAsString());
                }
            }
            else
            {
                UE_LOG(LogTelemetry, Display, TEXT("No response from ingestion server."));
            }
        });

        request->ProcessRequest();
    }

private:
    TUniquePtr<FRunnableThread> Thread;
    TUniquePtr<FEvent> Sync;
    double SendInterval;
    bool ShouldRun;
    bool IsComplete;
    FString IngestUrl;

    TCircularQueue<TSharedPtr<FTelemetryBuilder>> Pending;
};

static TUniquePtr<FTelemetryWorker> TelemetryWorker;

// Helper function to read configuration values from the ini
FString FTelemetryConfiguration::IniFileName = FString::Printf(TEXT("%sGameTelemetry.ini"), *FPaths::SourceConfigDir());
FString FTelemetryConfiguration::IniSectionName = TEXT("GameTelemetry");

FTelemetryConfiguration FTelemetryManager::GetConfigFromIni()
{
    FTelemetryConfiguration Config;
    FTelemetryConfiguration::GetString(TEXT("IngestUrl"), Config.IngestionUrl);
    FTelemetryConfiguration::GetDouble(TEXT("SendInterval"), Config.SendInterval);
    FTelemetryConfiguration::GetInt(TEXT("MaxBufferSize"), Config.PendingBufferSize);

    return Config;
}

// Set a new client id - Usually set to the platform client id
// If setting this again after initialization, consider flushing existing buffered telemetry or they may be incorrectly associated with the new id


// Set a new session id - Usually set when the game would like to differentiate between play sessions (such as changing users)
// If setting this more than once, consider flushing existing buffered telemetry or they may be incorrectly associated with the new id



void FTelemetryManager::Initialize(const FTelemetryConfiguration & Config)
{
    if (hasInit)
    {
        Instance->Shutdown();
    }

    Instance = MakeUnique<FTelemetryManager>(FTelemetryManager(Config));

    //Build type
    Instance->CommonProperties.SetProperty(L"build_type", EBuildConfigurations::ToString(FApp::GetBuildConfiguration()));

    //Platform
    Instance->CommonProperties.SetProperty(L"platform", FString(FPlatformProperties::IniPlatformName()));

    //Client ID
    Instance->CommonProperties.SetProperty(L"client_id", FPlatformMisc::GetLoginId());

    //Session ID
    Instance->CommonProperties.SetProperty(L"session_id", FApp::GetSessionId().ToString(EGuidFormats::DigitsWithHyphens));

    //Build ID
    Instance->CommonProperties.SetProperty(L"build_id", FApp::GetBuildVersion());

    //Process ID
    Instance->CommonProperties.SetProperty(L"process_id", FGenericPlatformProcess::GetCurrentProcessId());

    //User ID
    Instance->CommonProperties.SetProperty(L"user_id", FApp::GetSessionOwner());

    TelemetryWorker = MakeUnique<FTelemetryWorker>(Config.IngestionUrl, Config.SendInterval, Config.PendingBufferSize);
    hasInit = true;
}

void FTelemetryManager::Record(const FString &Name, const FString &Category, const FString &Version, FTelemetryBuilder &&Properties)
{
    if (hasInit)
    {
        TSharedPtr<FTelemetryBuilder> Evt(new FTelemetryBuilder(MoveTemp(Properties)));

        Evt->SetProperty(FTelemetry::ClientTimestamp());
        Evt->SetProperty(FTelemetry::EventName(Name));
        Evt->SetProperty(FTelemetry::Category(Category));
        Evt->SetProperty(FTelemetry::Version(Version));

        uint32 CurrentSequence = Sequence++;
        Evt->SetProperty(TEXT("seq"), FVariant(CurrentSequence));

        TelemetryWorker->Enqueue(Evt);
    }
    else
    {
        UE_LOG(LogTelemetry, Error, TEXT("Cannot record event because the telemetry subsystem has not been initialized."));
    }

}

inline void FTelemetryManager::SetClientId(const FString & InClientId)
{
    Instance->CommonProperties.SetProperty(FTelemetry::ClientId(InClientId));
}

inline void FTelemetryManager::SetSessionId(const FString & InSessionId)
{
    Instance->CommonProperties.SetProperty(FTelemetry::SessionId(InSessionId));
}


void FTelemetryManager::Shutdown()
{
    if (hasInit)
    {
        TelemetryWorker->Exit();
        TelemetryWorker.Release();
    }
}
