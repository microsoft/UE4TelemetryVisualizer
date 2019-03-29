//--------------------------------------------------------------------------------------
// TelemetryQuery.cpp
//
// Provides implementation of serializer
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#include "Query/TelemetryQuery.h"
#include "TelemetryVisualizerUI.h"
#include "HttpModule.h"
#include "Http.h"


// See: EQueryNodeType
static const FQueryNodeType NodeTypeStrings[] =
{
	"comparison",
	"group"
};

// See: EQueryOp
static const FQueryOperator QueryOpStrings[] =
{
	"eq",
	"gt",
	"gte",
	"lt",
	"lte",
	"neq",
	"in",
	"btwn",
	"and",
	"or",
};

static inline const FQueryOperator &GetOp(EQueryOp Op) { return QueryOpStrings[(int)Op]; }
static inline const FQueryOperator &GetType(EQueryNodeType Type) { return NodeTypeStrings[(int)Type]; }

FString FQuerySerializer::Serialize(const FQueryNodePtr Node)
{
	FString Payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Payload);


	Serialize(Node, Writer);

	Writer->Close();

	return Payload;
}

void WriteValue(const TSharedPtr<FJsonValue> &Value, TSharedRef<TJsonWriter<>> &Writer)
{
	switch (Value->Type)
	{
	case EJson::Number:
		Writer->WriteValue("value", Value->AsNumber());
		break;
	case EJson::String:
		Writer->WriteValue("value", Value->AsString());
		break;
	case EJson::Boolean:
		Writer->WriteValue("value", Value->AsBool());
		break;
	case EJson::Array:
		Writer->WriteArrayStart("values");
		for (const TSharedPtr<FJsonValue> &Item : Value->AsArray())
		{
			switch (Item->Type)
			{
			case EJson::Number:
				Writer->WriteValue(Item->AsNumber());
				break;
			case EJson::String:
				Writer->WriteValue(Item->AsString());
				break;
			case EJson::Boolean:
				Writer->WriteValue(Item->AsBool());
				break;
			}
		}
		Writer->WriteArrayEnd();
		break;
	}
}


void FQuerySerializer::Serialize(const FQueryNodePtr &Node, TSharedRef<TJsonWriter<>> &Writer)
{
	Writer->WriteObjectStart();
	Writer->WriteValue("type", GetType(Node->Type));
	Writer->WriteValue("op", GetOp(Node->Operator));
	{
		switch (Node->Type)
		{
		case EQueryNodeType::Group:
			Serialize(*(Node->Children), Writer);
			break;

		case EQueryNodeType::Comparison:
			Writer->WriteValue("column", Node->Column);
			WriteValue(Node->Value, Writer);
			break;
		}
	}
	Writer->WriteObjectEnd();
}

void FQuerySerializer::Serialize(FQueryNodeList &Nodes, TSharedRef<TJsonWriter<>> &Writer)
{
	Writer->WriteArrayStart("children");
	{
		for (FQueryNodePtr Item : Nodes)
		{
			Serialize(Item, Writer);
		}
	}
	Writer->WriteArrayEnd();
}

