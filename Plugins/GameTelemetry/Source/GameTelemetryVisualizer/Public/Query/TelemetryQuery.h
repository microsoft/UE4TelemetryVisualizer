//--------------------------------------------------------------------------------------
// TelemetryQuery.h
//
// Provides query interface for event lookup
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#include "CoreMinimal.h"
#include "Json.h"
#include "Http.h"
#include "TelemetryService.h"

#pragma once

struct SQueryResult;
DECLARE_DELEGATE_OneParam(QueryResultHandler, TSharedPtr<SQueryResult>);

using FQueryOperator = FString;
using FQueryNodeType = FString;

struct FQueryNode;

using FQueryNodePtr = TSharedPtr<FQueryNode>;
using FQueryNodeList = TArray<TSharedPtr<FQueryNode>>;


//Query identifier (single comparison or container of nodes)
enum class EQueryNodeType
{
	Comparison = 0,
	Group = 1
};

//Query operator
enum class EQueryOp
{
	Eq = 0,
	Gt = 1,
	Gte = 2,
	Lt = 3,
	Lte = 4,
	Neq = 5,
	In = 6,
	Btwn = 7,
	And = 8,
	Or = 9,
};

//JSON value converter
class FJsonValueUtil
{
public:

	static inline TSharedPtr<FJsonValue> Create(FString Value)
	{
		return TSharedPtr<FJsonValue>(new FJsonValueString(Value));
	}

	static inline TSharedPtr<FJsonValue> Create(FGuid Value)
	{
		return TSharedPtr<FJsonValue>(new FJsonValueString(Value.ToString()));
	}

	static inline TSharedPtr<FJsonValue> Create(double Value)
	{
		return TSharedPtr<FJsonValue>(new FJsonValueNumber(Value));
	}

	static inline TSharedPtr<FJsonValue> Create(float Value)
	{
		return TSharedPtr<FJsonValue>(new FJsonValueNumber(Value));
	}

	static inline TSharedPtr<FJsonValue> Create(int64 Value)
	{
		return TSharedPtr<FJsonValue>(new FJsonValueNumber(Value));
	}

	static inline TSharedPtr<FJsonValue> Create(int32 Value)
	{
		return TSharedPtr<FJsonValue>(new FJsonValueNumber(Value));
	}

	static inline TSharedPtr<FJsonValue> Create(int16 Value)
	{
		return TSharedPtr<FJsonValue>(new FJsonValueNumber(Value));
	}

	static inline TSharedPtr<FJsonValue> Create(int8 Value)
	{
		return TSharedPtr<FJsonValue>(new FJsonValueNumber(Value));
	}

	static inline TSharedPtr<FJsonValue> Create(uint64 Value)
	{
		return TSharedPtr<FJsonValue>(new FJsonValueNumber(Value));
	}

	static inline TSharedPtr<FJsonValue> Create(uint32 Value)
	{
		return TSharedPtr<FJsonValue>(new FJsonValueNumber(Value));
	}

	static inline TSharedPtr<FJsonValue> Create(uint16 Value)
	{
		return TSharedPtr<FJsonValue>(new FJsonValueNumber(Value));
	}

	static inline TSharedPtr<FJsonValue> Create(uint8 Value)
	{
		return TSharedPtr<FJsonValue>(new FJsonValueNumber(Value));
	}

	static inline TSharedPtr<FJsonValue> Create(const TArray<TSharedPtr<FJsonValue>> &Array)
	{
		return TSharedPtr<FJsonValue>(new FJsonValueArray(Array));
	}
};

//Nodes for query conditions and containers
struct FQueryNode
{
	EQueryNodeType Type;

	EQueryOp Operator;

	FString Column;

	TSharedPtr<FQueryNodeList> Children;

	TSharedPtr<FJsonValue> Value;
	
	// Standard single value node
	FQueryNode(EQueryNodeType Type, FString Column, EQueryOp Op, TSharedPtr<FJsonValue> &&Value) :
		Type(Type),
		Column(Column),
		Operator(Op),
		Value(MoveTemp(Value))
	{
	}

	// Overload for BETWEEN query nodes
	FQueryNode(EQueryNodeType Type, FString Column, EQueryOp Op, TSharedPtr<FJsonValue> &&Value1, TSharedPtr<FJsonValue> &&Value2) :
		Type(Type),
		Column(Column),
		Operator(Op),
		Value(FJsonValueUtil::Create(TArray<TSharedPtr<FJsonValue>>({ MoveTemp(Value1), MoveTemp(Value2) })))
	{
		// Between and In operators are the only ones that know to look at the Values array
		check(Op == EQueryOp::Btwn || Op == EQueryOp::In);
	}

	// Overload for grouping nodes (AND, OR)
	FQueryNode(EQueryNodeType Type, EQueryOp Op, FQueryNodeList &&ChildNodes) :
		Type(Type),
		Operator(Op),
		Children(new FQueryNodeList(MoveTemp(ChildNodes)))
	{
	}

private:
	bool ValidateBetweenRange(FJsonValue &Value1, FJsonValue &Value2)
	{
		if (Value1.Type != Value2.Type) { return false; }

		if (Value1.Type == EJson::Number)
		{
			return (Value1.AsNumber() <= Value2.AsNumber());
		}
		else if (Value1.Type == EJson::String)
		{
			return (Value1.AsString() <= Value2.AsString());
		}
		else
		{
			return false;
		}
	}
};

//Helper for building query nodes
class QBuilder
{
private:
	// Nodes for comparing a column to a single value (e.g. equals, greater than, etc.)
	template<typename T>
	static inline TSharedPtr<FQueryNode> CreateComparisonNode(FString Column, EQueryOp Op, T Value)
	{
		return TSharedPtr<FQueryNode>(new FQueryNode(EQueryNodeType::Comparison, Column, Op, FJsonValueUtil::Create(Value)));
	}

	// Nodes for comparing a column to multiple values (e.g. between)
	template<typename T>
	static inline TSharedPtr<FQueryNode> CreateComparisonNode(FString Column, EQueryOp Op, T Value1, T Value2)
	{
		return TSharedPtr<FQueryNode>(new FQueryNode(EQueryNodeType::Comparison, Column, Op, FJsonValueUtil::Create(Value1), FJsonValueUtil::Create(Value2)));
	}

	// Nodes for grouping multiple nodes together with a common query operator (e.g. and, or)
	static inline TSharedPtr<FQueryNode> CreateGroupNode(EQueryOp Op, FQueryNodeList &&ChildNodes)
	{
		return TSharedPtr<FQueryNode>(new FQueryNode(EQueryNodeType::Group, Op, MoveTemp(ChildNodes)));
	}


public:

	template<typename T>
	static inline TSharedPtr<FQueryNode> Eq(FString Column, T Value) { return CreateComparisonNode(Column, EQueryOp::Eq, Value); }

	template<typename T>
	static inline TSharedPtr<FQueryNode> Gt(FString Column, T Value) { return CreateComparisonNode(Column, EQueryOp::Gt, Value); }

	template<typename T>
	static inline TSharedPtr<FQueryNode> Gte(FString Column, T Value) { return CreateComparisonNode(Column, EQueryOp::Gte, Value); }

	template<typename T>
	static inline TSharedPtr<FQueryNode> Lt(FString Column, T Value) { return CreateComparisonNode(Column, EQueryOp::Lt, Value); }
	
	template<typename T>
	static inline TSharedPtr<FQueryNode> Lte(FString Column, T Value) { return CreateComparisonNode(Column, EQueryOp::Lte, Value); }
	
	template<typename T>
	static inline TSharedPtr<FQueryNode> Neq(FString Column, T Value) { return CreateComparisonNode(Column, EQueryOp::Neq, Value); }

	template<typename T>
	static inline TSharedPtr<FQueryNode> In(FString Column, TArray<T> &&Values) { return CreateComparisonNode(Column, EQueryOp::In, MoveTemp(Values)); }
	
	template<typename T>
	static inline TSharedPtr<FQueryNode> Btwn(FString Column, T LowerBound, T UpperBound) { return CreateComparisonNode(Column, EQueryOp::Btwn, LowerBound, UpperBound); }

	static inline TSharedPtr<FQueryNode> And(FQueryNodeList &&ChildNodes) { return CreateGroupNode(EQueryOp::And, MoveTemp(ChildNodes)); }
	
	static inline TSharedPtr<FQueryNode> Or(FQueryNodeList &&ChildNodes) { return CreateGroupNode(EQueryOp::Or, MoveTemp(ChildNodes)); }

};

//Node to JSON serializer
class FQuerySerializer
{
public:
	FString Serialize(const FQueryNodePtr Node);

private:

	void Serialize(const FQueryNodePtr &Node, TSharedRef<TJsonWriter<>> &Writer);
	void Serialize(FQueryNodeList &Nodes, TSharedRef<TJsonWriter<>> &Writer);
};

//Interface for standard event data
class ITelemetryEventData
{
public:
	virtual FString GetId() const = 0;
	virtual FString GetClientId() const = 0;
	virtual FString GetSessionId() const = 0;
	virtual FString GetName() const = 0;
	virtual FString GetBuildType() const = 0;
	virtual FString GetBuildId() const = 0;
	virtual FString GetPlatform() const = 0;
	virtual FString GetUserId() const = 0;
	virtual FString GetCategory() const = 0;

	virtual uint32 GetSequence() const = 0;
	virtual FDateTime GetTime() const = 0;

	virtual FVector GetPlayerPosition() const = 0;
	virtual FVector GetPlayerDirection() const = 0;
	virtual FVector GetCameraPosition() const = 0;
	virtual FVector GetCameraDirection() const = 0;

	virtual FString GetString(const FString &Name) const = 0;
	virtual double GetNumber(const FString &Name) const = 0;
	virtual FVector GetVector(const FString &BaseName) const = 0;
	virtual FDateTime GetDateTime(const FString &Name) const = 0;
	virtual bool GetBool(const FString &Name) const = 0;
};

//Wrapper for each event
class FSimpleEvent : public ITelemetryEventData
{
	const TCHAR *PLAYER_POS = TEXT("pos");
	const TCHAR *PLAYER_DIR = TEXT("dir");
	const TCHAR *CAM_POS = TEXT("cam_pos");
	const TCHAR *CAM_DIR = TEXT("cam_dir");
	const TCHAR *ID = TEXT("id");
	const TCHAR *CLIENT_ID = TEXT("client_id");
	const TCHAR *SESSION_ID = TEXT("session_id");
	const TCHAR *SEQUENCE = TEXT("seq");
	const TCHAR *EVENT_NAME = TEXT("name");
	const TCHAR *EVENT_ACTION = TEXT("action");
	const TCHAR *CLIENT_TIMESTAMP = TEXT("client_ts");
	const TCHAR *EVENT_VERSION = TEXT("e_ver");
	const TCHAR *BUILD_TYPE = TEXT("build_type");
	const TCHAR *BUILD_ID = TEXT("build_id");
	const TCHAR *PLATFORM = TEXT("platform");
	const TCHAR *USER_ID = TEXT("user_id");
	const TCHAR *CATEGORY = TEXT("cat");

	TMap<FString, TSharedPtr<FJsonValue>> Attributes;

public:
	virtual ~FSimpleEvent() {}

	FString GetId() const override { return GetString(ID); }
	FString GetClientId() const override { return GetString(CLIENT_ID); }
	FString GetSessionId() const override { return GetString(SESSION_ID); }
	FString GetName() const override { return GetString(EVENT_NAME); }
	FString GetBuildType() const override { return GetString(BUILD_TYPE); }
	FString GetBuildId() const override { return GetString(BUILD_ID); }
	FString GetPlatform() const override { return GetString(PLATFORM); }
	FString GetUserId() const override { return GetString(USER_ID); }
	FString GetCategory() const override { return GetString(CATEGORY); }

	uint32 GetSequence() const override { return (uint32)GetNumber(SEQUENCE); }

	FDateTime GetTime() const override
	{
		FDateTime Dt;
		FDateTime::ParseIso8601(*GetString(CLIENT_TIMESTAMP), Dt);
		return Dt;
	}

	FDateTime GetDateTime(const FString &Name) const override
	{
		FDateTime Dt;
		FDateTime::ParseIso8601(*Name, Dt);
		return Dt;
	}

	static FSimpleEvent Parse(TSharedPtr<FJsonObject> JObj)
	{
		FSimpleEvent ev;
		ev.Attributes.Append(JObj->Values);

		return ev;
	}

	FVector GetPlayerPosition() const override
	{
		return GetVector(PLAYER_POS);
	}

	FVector GetPlayerDirection() const override
	{
		return GetVector(PLAYER_DIR);
	}

	FVector GetCameraPosition() const override
	{
		return GetVector(CAM_POS);
	}

	FVector GetCameraDirection() const override
	{
		return GetVector(CAM_DIR);
	}

	bool GetString(const FString &Name, FString &OutString) const
	{
		TSharedPtr<FJsonValue> Value = Attributes.FindRef(Name);
		if (Value.IsValid())
		{
			OutString.Append(Value->AsString());
			return true;
		}
		return false;
	}

	FString GetString(const FString &Name) const override
	{
		FString Value;
		GetString(Name, Value);
		return Value;
	}

	bool GetNumber(const FString &Name, double &Number) const
	{
		TSharedPtr<FJsonValue> Value = Attributes.FindRef(Name);
		if (Value.IsValid())
		{
			Number = Value->AsNumber();
			return true;
		}
		return false;
	}

	double GetNumber(const FString &Name) const override
	{
		double Value;
		GetNumber(Name, Value);
		return Value;
	}

	bool GetVector(const FString &BaseName, FVector &Vector) const
	{
		float X, Y, Z;
		TSharedPtr<FJsonValue> Value = Attributes.FindRef(BaseName + TEXT("_x"));
		if (Value.IsValid())
		{
			X = Value->AsNumber();
		}
		else { return false; }

		Value = Attributes.FindRef(BaseName + TEXT("_y"));
		if (Value.IsValid())
		{
			Y = Value->AsNumber();
		}
		else { return false; }

		Value = Attributes.FindRef(BaseName + TEXT("_z"));
		if (Value.IsValid())
		{
			Z = Value->AsNumber();

			Vector.X = X;
			Vector.Y = Y;
			Vector.Z = Z;

			return true;
		}

		return false;
	}

	FVector GetVector(const FString &BaseName) const override
	{
		FVector Vector;
		GetVector(BaseName, Vector);
		return Vector;
	}

	bool GetBool(const FString &Name, bool &OutBool) const
	{
		TSharedPtr<FJsonValue> Value = Attributes.FindRef(Name);
		if (Value.IsValid())
		{
			double OutD;
			if (!Value->TryGetBool(OutBool))
			{
				if (!Value->TryGetNumber(OutD))
				{
					return false;
				}
				OutBool = (OutD == 1.0);
			}
			return true;
		}
		return false;
	}

	bool GetBool(const FString &Name) const
	{
		bool Value = false;
		GetBool(Name, Value);
		return Value;
	}
};

//Result data structure for query request
struct SQueryResult
{
	struct SQueryResultHeader
	{
		bool Success;
		int Count;
		long QueryTime;
	} Header;

	TArray<FSimpleEvent> Events;

	static TSharedPtr<SQueryResult> Parse(const FString &Response)
	{
		TSharedPtr<SQueryResult> Result(new SQueryResult);
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response);
		TSharedPtr<FJsonObject> RootObject;
		if (FJsonSerializer::Deserialize(Reader, RootObject))
		{
			FJsonObject &Root = *RootObject;
			TSharedPtr<FJsonObject> Header = Root.GetObjectField(TEXT("Header"));
			TArray<TSharedPtr<FJsonValue>> Events = Root.GetArrayField(TEXT("Results"));

			Result->Header.Success = Header->GetBoolField("Success");
			Result->Header.Count = Header->GetNumberField("Count");
			Result->Header.QueryTime = Header->GetNumberField("QueryTime");

			for (TSharedPtr<FJsonValue> Event : Events)
			{
				Result->Events.Add(FSimpleEvent::Parse(Event->AsObject()));
			}
		}

		return Result;
	}
};

//Query execution
class FQueryExecutor
{
public:
	FQueryExecutor() : IsInitialized(false) {}

	void ExecuteCustomQuery(const FString &QueryText, QueryResultHandler HandlerFunc)
	{
		ExecuteCustomQuery(QueryText, HandlerFunc, -1);
	}

	void ExecuteCustomQuery(const FString &QueryText, QueryResultHandler HandlerFunc, int32 TakeLimit)
	{
		if (!IsInitialized)
		{
			Initialize();
		}

		if (TakeLimit < 0)
		{
			TakeLimit = ConfiguredTakeLimit;
		}

		FHttpRequestPtr Request = CreateRequest(HandlerFunc, TakeLimit);
		Request->SetVerb(TEXT("POST"));
		Request->SetContentAsString(QueryText);
		Request->ProcessRequest();
	}

	void ExecuteCustomQuery(QueryResultHandler HandlerFunc)
	{
		ExecuteDefaultQuery(HandlerFunc, -1);
	}

	void ExecuteDefaultQuery(QueryResultHandler HandlerFunc, int32 TakeLimit)
	{
		if (!IsInitialized)
		{
			Initialize();
		}

		if (TakeLimit < 0)
		{
			TakeLimit = ConfiguredTakeLimit;
		}

		FHttpRequestPtr Request = CreateRequest(HandlerFunc, TakeLimit);
		Request->SetVerb(TEXT("GET"));
		Request->ProcessRequest();
	}

private:
	FHttpRequestPtr CreateRequest(QueryResultHandler HandlerFunc, int32 TakeLimit)
	{
		auto Request = FTelemetryService::CreateServiceRequest();

		FString Url = FString::Printf(TEXT("%s?take=%i"), *QueryUrl, TakeLimit);

		Request->SetURL(Url);
		Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
		Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");

		Request->OnProcessRequestComplete().BindLambda([HandlerFunc](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			if (bWasSuccessful)
			{
				TSharedPtr<SQueryResult> Result = SQueryResult::Parse(Response->GetContentAsString());

				HandlerFunc.ExecuteIfBound(Result);
			}
		});

		return Request;
	}

	void Initialize()
	{
		const FString IniName = FString::Printf(TEXT("%sGameTelemetry.ini"), *FPaths::SourceConfigDir());
		const FString SectionName = "GameTelemetry";

		GConfig->GetString(*SectionName, TEXT("QueryUrl"), QueryUrl, IniName);
		if (!GConfig->GetInt(*SectionName, TEXT("QueryTakeLimit"), ConfiguredTakeLimit, IniName))
		{
			ConfiguredTakeLimit = DefaultTakeLimit;
		}

		IsInitialized = true;
	}

private:
	FString QueryUrl;

	bool IsInitialized;
	int32 ConfiguredTakeLimit;

	// Default max number of documents to retrieve from the server
	static const int32 DefaultTakeLimit = 10000;
};
