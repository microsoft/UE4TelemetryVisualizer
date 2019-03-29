//--------------------------------------------------------------------------------------
// TelemetryVisualizerUI.cpp
//
// Provides interactivity for Telemetry events
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#include "TelemetryVisualizerUI.h"
#include "Modules/ModuleManager.h"
#include "IMeshMergeUtilities.h"
#include "MeshMergeModule.h"
#include "Engine/StaticMeshActor.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"
#include "Async/Async.h"
#include "Query/TelemetryQuery.h"

#define LOCTEXT_NAMESPACE "Telemetry"

void FTelemetryVisualizerUI::Initialize()
{
	//Editor states
	m_editorDrawTarget = nullptr;
	m_gameDrawTarget = nullptr;
	m_PIEDrawTarget = nullptr;
	m_registeredWidget = false;
	m_needsActorUpdate = true;

	//Default event list
	m_anim_scrollBarLocation = 0.f;
	m_eventgroupList.Add(MakeShareable<FString>(new FString("")));
	m_vizSelection = m_eventgroupList[0];

	//Default heatmap state
	m_heatmapType = HeatmapType::Population;
	m_heatmapSize = MinHeatmapSize * 4;
	m_heatmapShapeType = EventType::Cube;
	m_material = nullptr;
	m_heatmapMinValue = -1;
	m_heatmapMaxValue = -1;
	m_heatmapOrientation = false;
	m_heatmapAnimation = false;

	//Default list of render shapes
	for (int i = 0; i < ShapeListStrings.Num(); i++)
	{
		m_shapeList.Add(MakeShareable<FString>(new FString(ShapeListStrings[i])));
	}

	//Default list of heatmap types
	for (int i = 0; i < HeatmapTypeStrings.Num(); i++)
	{
		m_heatmapTypeList.Add(MakeShareable<FString>(new FString(HeatmapTypeStrings[i])));
	}

	//Default list of query fields
	for (int i = 0; i < QueryFieldStrings.Num(); i++)
	{
		m_queryFieldList.Add(MakeShareable<FString>(new FString(QueryFieldStrings[i])));
	}

	//Default list of query operators
	for (int i = 0; i < QueryOperatorStrings.Num(); i++)
	{
		m_queryOperatorList.Add(MakeShareable<FString>(new FString(QueryOperatorStrings[i])));
	}

	//Default list of query and/or
	for (int i = 0; i < AndOrStrings.Num(); i++)
	{
		m_queryAndOrList.Add(MakeShareable<FString>(new FString(AndOrStrings[i])));
	}

	//Add required default query condition
	AddClause(-1);

	//Add tab spawner
	TSharedPtr<FWorkspaceItem> WorkspaceMenuCategory = FGlobalTabmanager::Get()->AddLocalWorkspaceMenuCategory(LOCTEXT("Telemetry_Telemetry", "Telemetry"));
	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FGlobalTabmanager::Get()->RegisterTabSpawner(TelemetryDataIndirectTabName, FOnSpawnTab::CreateRaw(this, &FTelemetryVisualizerUI::SpawnDataTab))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetDisplayName(FText::FromString(TEXT("Telemetry Data Viewer")));

	FGlobalTabmanager::Get()->RegisterTabSpawner(TelemetryVizIndirectTabName, FOnSpawnTab::CreateRaw(this, &FTelemetryVisualizerUI::SpawnVizTab))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetDisplayName(FText::FromString(TEXT("Telemetry Visualization Tools")));

	//Add editor UI extensions
	if (!IsRunningCommandlet() && !IsRunningGame())
	{
		TelemetryVisualizerCommands::Register();

		m_pluginCommands = MakeShareable(new FUICommandList);

		m_pluginCommands->MapAction(
			TelemetryVisualizerCommands::Get().m_displayDataTab,
			FExecuteAction::CreateRaw(this, &FTelemetryVisualizerUI::ShowDataTab),
			FCanExecuteAction());

		m_pluginCommands->MapAction(
			TelemetryVisualizerCommands::Get().m_displayVizTab,
			FExecuteAction::CreateRaw(this, &FTelemetryVisualizerUI::ShowVizTab),
			FCanExecuteAction());

		m_pluginCommands->MapAction(
			TelemetryVisualizerCommands::Get().m_displayAll,
			FExecuteAction::CreateRaw(this, &FTelemetryVisualizerUI::ShowAll),
			FCanExecuteAction());

		m_menuExtender = MakeShareable(new FExtender);
		m_menuExtension = m_menuExtender->AddMenuExtension("General", EExtensionHook::After, m_pluginCommands, FMenuExtensionDelegate::CreateRaw(this, &FTelemetryVisualizerUI::AddMenuExtensions));

		FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(m_menuExtender);

		m_extensionManager = LevelEditorModule.GetMenuExtensibilityManager();
	}

	//Setup timer
	m_tickDelegateHandle = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &FTelemetryVisualizerUI::UpdateDraw));
	m_startPIEDelegateHandle = FEditorDelegates::PostPIEStarted.AddRaw(this, &FTelemetryVisualizerUI::PIEStarted);
	m_endPIEDelegateHandle = FEditorDelegates::EndPIE.AddRaw(this, &FTelemetryVisualizerUI::PIEEnded);
}

void FTelemetryVisualizerUI::Shutdown()
{
	FTicker::GetCoreTicker().RemoveTicker(m_tickDelegateHandle);
	FEditorDelegates::PostPIEStarted.Remove(m_startPIEDelegateHandle);
	FEditorDelegates::PostPIEStarted.Remove(m_endPIEDelegateHandle);

	FGlobalTabmanager::Get()->UnregisterTabSpawner(TelemetryDataIndirectTabName);
	FGlobalTabmanager::Get()->UnregisterTabSpawner(TelemetryVizIndirectTabName);
	
	//Remove editor UI extensions
	if (m_extensionManager.IsValid())
	{
		TelemetryVisualizerCommands::Unregister();

		m_menuExtender->RemoveExtension(m_menuExtension.ToSharedRef());

		m_extensionManager->RemoveExtender(m_menuExtender);
	}
	else
	{
		m_extensionManager.Reset();
	}
}

void FTelemetryVisualizerUI::AddMenuExtensions(FMenuBuilder &builder)
{
	//Add editor UI elements
	builder.BeginSection("Telemetry Settings", LOCTEXT("Telemetry Settings", "Telemetry Settings"));
	builder.AddMenuEntry(TelemetryVisualizerCommands::Get().m_displayDataTab);
	builder.AddMenuEntry(TelemetryVisualizerCommands::Get().m_displayVizTab);
	builder.AddMenuEntry(TelemetryVisualizerCommands::Get().m_displayAll);
	builder.EndSection();
}

TSharedRef<SWidget> FTelemetryVisualizerUI::MakeWidgetForOption(TSharedPtr<FString> InOption)
{
	return SNew(STextBlock).Text(FText::FromString(*InOption));
}

/////////////////////////////////DRAW TO WORLD/////////////////////////////////
UWorld* FTelemetryVisualizerUI::GetLocalWorld()
{
	UWorld* foundWorld = nullptr;

	if (m_gameDrawTarget)
	{
		foundWorld = m_gameDrawTarget;
	}
	else if (m_PIEDrawTarget)
	{
		foundWorld = m_PIEDrawTarget;
	}
	else if (m_editorDrawTarget)
	{
		foundWorld = m_editorDrawTarget;
	}

	return foundWorld;
}

bool FTelemetryVisualizerUI::UpdateDraw(float val)
{
	UWorld* currentTarget = GetLocalWorld();

	if (currentTarget == nullptr)
	{
		UpdateGameState(true);
	}

	if (currentTarget && m_filterCollection.Num() > 0)
	{
		DrawTelemetry(currentTarget, m_filterCollection);
	}

	return true;
}

void FTelemetryVisualizerUI::UpdateGameState(bool allowPIE)
{
	UWorld* currentTarget = nullptr;
	TIndirectArray<FWorldContext> worldContexts = GEngine->GetWorldContexts();

	m_editorDrawTarget = nullptr;
	m_gameDrawTarget = nullptr;
	m_PIEDrawTarget = nullptr;

	for (int i = 0; i < worldContexts.Num(); i++)
	{
		currentTarget = worldContexts[i].World();

		if (currentTarget)
		{
			switch (currentTarget->WorldType)
			{
			case EWorldType::PIE:
				if (allowPIE) m_PIEDrawTarget = currentTarget;
				break;
			case EWorldType::Editor:
				m_editorDrawTarget = currentTarget;
				break;
			case EWorldType::Game:
				m_gameDrawTarget = currentTarget;
				break;
			}
		}
	}
}

void FTelemetryVisualizerUI::CreateActor(UWorld* drawTarget, int index, TSharedPtr<STelemetryEvent> data, FColor color, EventType shape)
{
	ATelemetryEvent* tempActor;
	FString tempName;

	tempName = data->name + FString::FromInt(index);

	FActorSpawnParameters params;
	params.Name = *tempName;

	FRotator tempRot = FRotator::ZeroRotator;
	if (data->orientation != FVector::ZeroVector)
	{
		tempRot = data->orientation.Rotation();
	}

	tempActor = drawTarget->SpawnActor<ATelemetryEvent>(data->point, tempRot, params);
	tempActor->SetActorLabel(*tempName);

	tempActor->AddEvent(data->point, data->orientation, color, shape);
	tempActor->SetEvent(data);

	tempName = "/TelemetryEvents/" + FString(data->name);
	tempActor->SetFolderPath(*tempName);

	m_eventActors.Add(tempActor);
}

void FTelemetryVisualizerUI::CreateActors(UWorld* drawTarget, TArray<TSharedPtr<STelemetryEvent>> data, FColor color, EventType shape)
{
	ATelemetryEvent* tempActor;
	FString tempName;

	for (int i = 0; i < data.Num(); i++)
	{
		tempName = data[i]->name + FString::FromInt(i);

		FActorSpawnParameters params;
		params.Name = *tempName;

		FRotator tempRot = FRotator::ZeroRotator;
		if (data[i]->orientation != FVector::ZeroVector)
		{
			tempRot = data[i]->orientation.Rotation();
		}

		tempActor = drawTarget->SpawnActor<ATelemetryEvent>(data[i]->point, tempRot, params);
		tempActor->SetActorLabel(*tempName);

		tempActor->AddEvent(data[i]->point, data[i]->orientation, color, shape);
		tempActor->SetEvent(data[i]);

		tempName = "/TelemetryEvents/" + FString(data[i]->name);
		tempActor->SetFolderPath(*tempName);

		m_eventActors.Add(tempActor);
	}
}

void FTelemetryVisualizerUI::CreateActors(UWorld* drawTarget, TArray<TSharedPtr<STelemetryEvent>> data, int start, int end, FColor color, EventType shape)
{
	ATelemetryEvent* tempActor;
	FString tempName;

	for (int i = start; i < end; i++)
	{
		tempName = data[i]->name + FString::FromInt(i);

		FActorSpawnParameters params;
		params.Name = *tempName;

		tempActor = drawTarget->SpawnActor<ATelemetryEvent>(data[i]->point, FRotator::ZeroRotator, params);
		tempActor->SetActorLabel(*tempName);

		tempActor->AddEvent(data[i]->point, data[i]->orientation, color, shape);
		tempActor->SetEvent(data[i]);

		tempName = "/TelemetryEvents/" + FString(data[i]->name);
		tempActor->SetFolderPath(*tempName);

		m_eventActors.Add(tempActor);
	}
}

void FTelemetryVisualizerUI::DestroyActors()
{
	UWorld* currentWorld = GetLocalWorld();

	if (currentWorld != nullptr)
	{
		for (int i = 0; i < m_eventActors.Num(); i++)
		{
			currentWorld->DestroyActor(m_eventActors[i]);
		}
	}

	m_eventActors.Empty();
}

void FTelemetryVisualizerUI::DestroyLastActor()
{
	UWorld* currentWorld = GetLocalWorld();

	if (currentWorld != nullptr && m_eventActors.Num() > 0)
	{
		currentWorld->DestroyActor(m_eventActors.Last());
		m_eventActors.RemoveAt(m_eventActors.Num() - 1);
	}
}

void FTelemetryVisualizerUI::DrawPoint(UWorld* drawTarget, TSharedPtr<STelemetryEvent> data, FColor color)
{
	DrawDebugPoint(drawTarget, data->point, 5.f, color, true);
}

void FTelemetryVisualizerUI::DrawPoints(UWorld* drawTarget, TArray<TSharedPtr<STelemetryEvent>> data, FColor color)
{
	DrawPoints(drawTarget, data, 0, data.Num(), color);
}

void FTelemetryVisualizerUI::DrawPoints(UWorld* drawTarget, TArray<TSharedPtr<STelemetryEvent>> data, int start, int end, FColor color)
{
	for (int i = start; i < end; i++)
	{
		DrawPoint(drawTarget, (TSharedPtr<STelemetryEvent>)data[i], color);
	}
}

void FTelemetryVisualizerUI::DrawSphere(UWorld* drawTarget, TSharedPtr<STelemetryEvent> data, FColor color)
{
	DrawDebugSphere(drawTarget, data->point, 26.f, 12, color, true);
}

void FTelemetryVisualizerUI::DrawSpheres(UWorld* drawTarget, TArray<TSharedPtr<STelemetryEvent>> data, FColor color)
{
	DrawSpheres(drawTarget, data, 0, data.Num(), color);
}

void FTelemetryVisualizerUI::DrawSpheres(UWorld* drawTarget, TArray<TSharedPtr<STelemetryEvent>> data, int start, int end, FColor color)
{
	for (int i = start; i < end; i++)
	{
		DrawSphere(drawTarget, (TSharedPtr<STelemetryEvent>)data[i], color);
	}
}

void FTelemetryVisualizerUI::DrawCube(UWorld* drawTarget, TSharedPtr<STelemetryEvent> data, FColor color)
{
	DrawDebugBox(drawTarget, data->point, FVector(5.f, 5.f, 5.f), color, true);
}

void FTelemetryVisualizerUI::DrawCubes(UWorld* drawTarget, TArray<TSharedPtr<STelemetryEvent>> data, FColor color)
{
	DrawCubes(drawTarget, data, 0, data.Num(), color);
}

void FTelemetryVisualizerUI::DrawCubes(UWorld* drawTarget, TArray<TSharedPtr<STelemetryEvent>> data, int start, int end, FColor color)
{
	for (int i = start; i < end; i++)
	{
		DrawCube(drawTarget, (TSharedPtr<STelemetryEvent>)data[i], color);
	}
}

void FTelemetryVisualizerUI::DrawTelemetry(UWorld* drawTarget, TArray<SEventEditorContainer*> data)
{
	if (!m_anim_Control.IsStopped())
	{
		if (m_anim_Control.ShouldAnimate())
		{
			if (m_anim_Control.NeedRefresh())
			{
				DestroyActors();

				//If we are playing in reverse, we need to add the remaining points to be removed later
				if (m_anim_Control.GetPlaySpeed() < 0)
				{
					SEventEditorContainer* eventContainer = m_anim_Control.GetEventContainer();
					int j = 0;
					if (eventContainer != nullptr)
					{
						for (auto currEvent : eventContainer->events)
						{
							CreateActor(drawTarget, j, (TSharedPtr<STelemetryEvent>)currEvent, m_anim_Control.GetColor(), m_anim_Control.GetShapeType());
							j++;
						}
					}
				}

				FlushPersistentDebugLines(drawTarget);
			}

			TArray<TSharedPtr<STelemetryEvent>> tempArray;

			if (m_anim_Control.GetPlaySpeed() >= 0)
			{
				if (m_heatmapAnimation)
				{
					//Regenerate the heatmap only using a subset of the events
					int start = m_anim_Control.GetNextIndex();
					int next = m_anim_Control.GetNextEventCount();
					SEventEditorContainer* tempContainer = m_anim_Control.GetEventContainer();
					m_anim_scrollBarLocation = m_anim_Control.GetTimeScaleFromTime();
					if (start != next)
					{
						GenerateHeatmap(tempContainer, next, tempContainer->events.Num() - 1);
					}
				}
				else
				{
					//Update forward point animation by getting events that have occured over the last timespan and creating them
					tempArray = m_anim_Control.GetNextEvents();
					int i = m_anim_Control.GetNextIndex();
					m_anim_scrollBarLocation = m_anim_Control.GetTimeScaleFromTime();
					for (auto currEvent : tempArray)
					{
						CreateActor(drawTarget, i, (TSharedPtr<STelemetryEvent>)currEvent, m_anim_Control.GetColor(), m_anim_Control.GetShapeType());
						i++;
					}
				}
			}
			else
			{
				if (m_heatmapAnimation)
				{
					//Regenerate the heatmap only using a subset of the events
					int start = m_anim_Control.GetPrevIndex();
					int next = m_anim_Control.GetPrevEventCount();
					SEventEditorContainer* tempContainer = m_anim_Control.GetEventContainer();
					m_anim_scrollBarLocation = 1 + m_anim_Control.GetTimeScaleFromTime();
					if (start != next)
					{
						GenerateHeatmap(tempContainer, next, tempContainer->events.Num() - 1);
					}
				}
				else
				{
					//Update reverse point animation by removing the last actor for as many events occured over the last timespan
					tempArray = m_anim_Control.GetPrevEvents();
					m_anim_scrollBarLocation = 1 + m_anim_Control.GetTimeScaleFromTime();
					for (auto currEvent : tempArray)
					{
						DestroyLastActor();
					}
				}
			}

			//Set the location of the slider
			if (!m_anim_Control.ShouldAnimate() && m_anim_Control.GetPlaySpeed() >= 0)
			{
				m_animationSlider->SetValue(1.f);
			}
			else if (!m_anim_Control.ShouldAnimate() && m_anim_Control.GetPlaySpeed() < 0)
			{
				m_animationSlider->SetValue(0.f);
			}
			else
			{
				m_animationSlider->SetValue(m_anim_scrollBarLocation);
			}

			//Update the current playtime
			if (m_animationCurrentTime.IsValid())
			{
				FNumberFormattingOptions format;
				format.MinimumIntegralDigits = 2;

				FTimespan totalTime = m_anim_Control.GetCurrentPlayTime();
				m_animationCurrentTime->SetText(FText::Format(FTextFormat::FromString("{0}:{1}"), FText::AsNumber(totalTime.GetMinutes()), FText::AsNumber(totalTime.GetSeconds(), &format)));
			}
		}
	}
	else
	{
		if (m_needsActorUpdate)
		{
			//Draw data points
			m_needsActorUpdate = false;
			DestroyActors();
			FlushPersistentDebugLines(drawTarget);

			for (auto events : m_filterCollection)
			{
				if (events->ShouldDraw())
				{
					int startIndex = 0;
					int endIndex = events->events.Num();

					CreateActors(drawTarget, events->events, startIndex, endIndex, events->GetColor(), events->GetShapeType());
				}
			}
		}
	}

	GEditor->RedrawAllViewports();
}

#undef LOCTEXT_NAMESPACE
