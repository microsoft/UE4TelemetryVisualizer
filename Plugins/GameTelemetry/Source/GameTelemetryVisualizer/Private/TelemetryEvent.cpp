//--------------------------------------------------------------------------------------
// TelemetryEvent.cpp
//
// Telemetry actor.  Provides interactive representation of an event
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#include "TelemetryEvent.h"
#include "TelemetryVisualizerPCH.h"
#include "TelemetryVisualizerTypes.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

ATelemetryEvent::ATelemetryEvent(const FObjectInitializer& InInitializer)
: AActor(InInitializer)
{
	//Cannot move this actor or interact in play
	bLockLocation = true;
	bCanBeDamaged = false;
	bIsEditorOnlyActor = true;
	SetActorEnableCollision(false);

	//Default scale
	scale = FVector(0.5f);

	//Cache pointer for material
	static ConstructorHelpers::FObjectFinder<UMaterial> MaterialAsset(TEXT("/GameTelemetry/Materials/M_Telemetry.M_Telemetry"));
	if (MaterialAsset.Succeeded())
	{
		MeshMaterial = MaterialAsset.Object;
	}

	//Cache pointers each possible mesh.  This call is required to be in the constructor
	ConstructorHelpers::FObjectFinder<UStaticMesh> MeshConeAsset(TEXT("/Engine/BasicShapes/Cone.Cone"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> MeshCubeAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> MeshCylinderAsset(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> MeshPlaneAsset(TEXT("/Engine/BasicShapes/Plane.Plane"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> MeshSphereAsset(TEXT("/Engine/BasicShapes/Sphere.Sphere"));

	if (MeshConeAsset.Succeeded())
	{
		MeshCone = MeshConeAsset.Object;
	}
	if (MeshCubeAsset.Succeeded())
	{
		MeshCube = MeshCubeAsset.Object;
	}
	if (MeshCylinderAsset.Succeeded())
	{
		MeshCylinder = MeshCylinderAsset.Object;
	}
	if (MeshPlaneAsset.Succeeded())
	{
		MeshPlane = MeshPlaneAsset.Object;
	}
	if (MeshSphereAsset.Succeeded())
	{
		MeshSphere = MeshSphereAsset.Object;
	}
}

void ATelemetryEvent::AddEvent(FVector inLocation, FVector inOrientation, FColor inColor, EventType inType, FBox inScale, float ratio)
{
	UInstancedStaticMeshComponent* NewComponent = nullptr;

	scale = inScale.GetExtent();

	for (int i = 0; i < InstanceTypes.Num(); i++)
	{
		if (inColor == InstanceTypes[i].color && inType == InstanceTypes[i].type)
		{
			NewComponent = InstanceTypes[i].component;
		}
	}

	FString tempName;

	if (NewComponent == nullptr)
	{
		if (GetRootComponent())
		{
			// Attach to root if we already have one
			NewComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		}
		else
		{
			// Make a new root if we dont have a root already
			SetRootComponent(NewComponent);
		}

		if (ratio < 2.f)
		{
			tempName = "HeatmapGroup_" + FString::SanitizeFloat(ratio);
		}
		else
		{
			tempName = "Mesh";
		}

		NewComponent = NewObject<UInstancedStaticMeshComponent>(this, *tempName);

		//Set mesh
		UStaticMesh* newMesh = nullptr;

		switch (inType)
		{
		case EventType::Cone:
			newMesh = MeshCone;
			break;
		case EventType::Cube:
			newMesh = MeshCube;
			break;
		case EventType::Cylinder:
			newMesh = MeshCylinder;
			break;
		case EventType::Plane:
			newMesh = MeshPlane;
			break;
		case EventType::Sphere:
			newMesh = MeshSphere;
			break;
		}

		if (newMesh != nullptr)
		{
			NewComponent->SetStaticMesh(newMesh);
			NewComponent->SetWorldScale3D(scale);
		}

		//Set material
		UMaterialInstanceDynamic* matInstance = NewComponent->CreateDynamicMaterialInstance(0, MeshMaterial);

		if (matInstance != nullptr)
		{
			matInstance->SetVectorParameterValue("Color", FLinearColor(inColor));
			NewComponent->SetMaterial(0, matInstance);
		}

		// Take 'instanced' ownership so it persists with this actor
		RemoveOwnedComponent(NewComponent);
		NewComponent->CreationMethod = EComponentCreationMethod::Instance;
		AddOwnedComponent(NewComponent);
		
		NewComponent->RegisterComponent();

		InstanceTypes.Add(InstanceType(NewComponent, inColor, inType));
	}

	FRotator tempRot = FRotator::ZeroRotator;
	if (inOrientation != FVector::ZeroVector)
	{
		tempRot = inOrientation.ToOrientationRotator();
		tempRot.Pitch -= 90.f;
	}

	NewComponent->AddInstanceWorldSpace(FTransform(tempRot, inLocation + inScale.GetCenter(), scale));
}

void ATelemetryEvent::SetEvent(const TSharedPtr<STelemetryEvent> inPointer)
{
	if (InstanceTypes.Num() != 1) return;

	time = inPointer->time;
	value = inPointer->value;
	location = inPointer->point;
	session = inPointer->session;
	orientation = inPointer->orientation;
	user = inPointer->user;
	build = inPointer->build;
	name = inPointer->name;
	category = inPointer->category;

	eventName = category + L" " + name;
}

void ATelemetryEvent::SetColor(FColor inColor)
{
	if (InstanceTypes.Num() != 1) return;

	UMaterialInterface * Material;
	UMaterialInstanceDynamic* matInstance;

	Material = InstanceTypes[0].component->GetMaterial(0);
	matInstance = InstanceTypes[0].component->CreateDynamicMaterialInstance(0, Material);

	if (matInstance != nullptr)
	{
		matInstance->SetVectorParameterValue("Color", FLinearColor(inColor));
		InstanceTypes[0].component->SetMaterial(0, matInstance);
	}
};

void ATelemetryEvent::SetShapeType(EventType inType)
{
	if (InstanceTypes.Num() != 1) return;

	UStaticMesh* newMesh = nullptr;

	switch (inType)
	{
	case EventType::Cone:
		newMesh = MeshCone;
		break;
	case EventType::Cube:
		newMesh = MeshCube;
		break;
	case EventType::Cylinder:
		newMesh = MeshCylinder;
		break;
	case EventType::Plane:
		newMesh = MeshPlane;
		break;
	case EventType::Sphere:
		newMesh = MeshSphere;
		break;
	}

	if (newMesh != nullptr)
	{
		InstanceTypes[0].component->SetStaticMesh(newMesh);
		InstanceTypes[0].component->SetWorldScale3D(scale);
	}
}

void ATelemetryEvent::SetScale(float inScale)
{
	if (InstanceTypes.Num() != 1) return;

	scale = FVector(inScale);

	InstanceTypes[0].component->SetWorldScale3D(scale);
}
