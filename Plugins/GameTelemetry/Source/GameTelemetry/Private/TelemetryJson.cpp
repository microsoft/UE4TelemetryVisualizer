//--------------------------------------------------------------------------------------
// TelemetryJson.cpp
//
// Json serialization for telemetry events.
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#include "TelemetryJson.h"
#include "TelemetryPCH.h"


void FTelemetryJsonSerializer::Serialize(const FTelemetryProperties &Container, const TSharedRef<TJsonWriter<>> &Writer)
{
	for (const FTelemetryProperty &Property : Container)
	{
		Serialize(Property, Writer);
	}
}

void FTelemetryJsonSerializer::Serialize(const FTelemetryProperty &Property, const TSharedRef<TJsonWriter<>> &Writer)
{
	switch (Property.Value.GetType())
	{
		case EVariantTypes::Empty:
			Writer->WriteValue(Property.Key, TEXT(""));
			break; // todo(scmatlof): error type
		
		// Dates
		case EVariantTypes::DateTime:
			Writer->WriteValue(Property.Key, Property.Value.GetValue<FDateTime>().ToIso8601());
			break;
		
		// Misc
		case EVariantTypes::Guid:
			Writer->WriteValue(Property.Key, Property.Value.GetValue<FGuid>().ToString());
			break;
		case EVariantTypes::ByteArray: // todo(scmatlof): Support base64?
			Writer->WriteValue(Property.Key, FBase64::Encode(Property.Value.GetBytes()));
			break;

		// Numerics
		case EVariantTypes::Bool:
			Writer->WriteValue<uint8>(Property.Key, Property.Value.GetValue<bool>() ? 1 : 0);
			break;
		case EVariantTypes::UInt8:
			Writer->WriteValue<int64>(Property.Key, Property.Value.GetValue<uint8>());
			break;
		case EVariantTypes::UInt16:
			Writer->WriteValue<int64>(Property.Key, Property.Value.GetValue<uint16>());
			break;
		case EVariantTypes::UInt32:
			Writer->WriteValue<int64>(Property.Key, Property.Value.GetValue<uint32>());
			break;
		case EVariantTypes::UInt64:
			Writer->WriteValue<int64>(Property.Key, Property.Value.GetValue<uint64>());
			break;
		case EVariantTypes::Int8:
			Writer->WriteValue<int8>(Property.Key, Property.Value.GetValue<int8>());
			break;
		case EVariantTypes::Int16:
			Writer->WriteValue<int16>(Property.Key, Property.Value.GetValue<int16>());
			break;
		case EVariantTypes::Int32:
			Writer->WriteValue<int32>(Property.Key, Property.Value.GetValue<int32>());
			break;
		case EVariantTypes::Int64:
			Writer->WriteValue<int64>(Property.Key, Property.Value.GetValue<int64>());
			break;
		case EVariantTypes::Float:
			Writer->WriteValue<float>(Property.Key, Property.Value.GetValue<float>());
			break;
		case EVariantTypes::Double:
			Writer->WriteValue<double>(Property.Key, Property.Value.GetValue<double>());
			break;

		// Strings
		/*case EVariantTypes::Ansichar:
			Writer->WriteValue(Property.Key, Property.Value.GetValue<ANSICHAR>());
			break;
		case EVariantTypes::Widechar:
			Writer->WriteValue(Property.Key, Property.Value.GetValue<WIDECHAR>());
			break;*/
		case EVariantTypes::String:
			Writer->WriteValue(Property.Key, Property.Value.GetValue<FString>());
			break;
		/*case EVariantTypes::Property.Key:
			Writer->WriteValue(Property.Key, Property.Value.GetValue<FProperty.Key>().);
			break;*/
		
		// Vectors
		case EVariantTypes::Vector:
		{
			FVector V(Property.Value.GetValue<FVector>());
			Writer->WriteValue(Property.Key + "_x", V.X);
			Writer->WriteValue(Property.Key + "_y", V.Y);
			Writer->WriteValue(Property.Key + "_z", V.Z);
		}
		break;
		case EVariantTypes::Vector2d:
		{
			FVector2D V(Property.Value.GetValue<FVector2D>());
			Writer->WriteValue(Property.Key + "_x", V.X);
			Writer->WriteValue(Property.Key + "_y", V.Y);
		}
		break;
		case EVariantTypes::Vector4:
		{
			FVector4 V(Property.Value.GetValue<FVector4>());
			Writer->WriteValue(Property.Key + "_x", V.X);
			Writer->WriteValue(Property.Key + "_y", V.Y);
			Writer->WriteValue(Property.Key + "_z", V.Z);
			Writer->WriteValue(Property.Key + "_w", V.W);
		}
		break;
		case EVariantTypes::IntVector:
		{
			FIntVector V(Property.Value.GetValue<FIntVector>());
			Writer->WriteValue(Property.Key + "_x", V.X);
			Writer->WriteValue(Property.Key + "_y", V.Y);
			Writer->WriteValue(Property.Key + "_z", V.Z);
		}
		break;

		default:
			break;
	}
}