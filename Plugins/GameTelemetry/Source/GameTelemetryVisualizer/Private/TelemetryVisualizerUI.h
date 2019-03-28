//--------------------------------------------------------------------------------------
// TelemetryVisualizerUI.h
//
// Provides interactivity for Telemetry events
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "TelemetryVisualizerCommands.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Colors/SColorPicker.h"
#include "Widgets/Input/SSlider.h"
#include "Widgets/Docking/SDockableTab.h"
#include "LevelEditor.h"
#include "Editor.h"
#include "DrawDebugHelpers.h"
#include "Containers/Ticker.h"
#include "TelemetryEvent.h"
#include "Query/TelemetryQuery.h"
#include "Slate.h"
#include "TelemetryVisualizerTypes.h"
#include "Query/TelemetryQuery.h"

static const float MinHeatmapSize = 50.f;
static const float MaxHeatmapSize = 1000.f;

class FTelemetryVisualizerUI
{
public:
	FTelemetryVisualizerUI() {}
	virtual ~FTelemetryVisualizerUI() {};

	//Plugin state
	void Initialize();
	void Shutdown();

	//Plugin state
	UWorld* GetLocalWorld();
	bool UpdateDraw(float val);
	void UpdateGameState(bool allowPIE);
	void PIEStarted(bool val) { UpdateGameState(true); };
	void PIEEnded(bool val) { UpdateGameState(false); };

	//Tab control
	TSharedRef<SDockTab> SpawnDataTab(const FSpawnTabArgs& TabSpawnArgs);
	TSharedRef<SDockTab> SpawnVizTab(const FSpawnTabArgs& TabSpawnArgs);
	void ShowDataTab() { FGlobalTabmanager::Get()->InvokeTab(TelemetryDataIndirectTabName); }
	void ShowVizTab() { FGlobalTabmanager::Get()->InvokeTab(TelemetryVizIndirectTabName); }
	void ShowAll()
	{
		ShowDataTab();
		ShowVizTab();
	}

	TSharedRef<SWidget> MakeWidgetForOption(TSharedPtr<FString> InOption);
	void GenerateEventBox();

	//Event Collection
	void QueryResults(TSharedPtr<SQueryResult> results);
	FReply UpdateFilterEvents();
	int FilterEvents();
	void CollectEvents(FQueryNodePtr query);
	void GenerateScrollBoxes(int count);

	//Viz tab event selection
	void OnVizSelectionChanged(TSharedPtr<FString> NewValue, ESelectInfo::Type);
	FText GetCurrentVizItem() const;

	//Query Scroll box
	void GenerateQueryScrollBox();
	FText GetAndOrItem(int index) const;
	void OnAndOrSelectionChanged(TSharedPtr<FString> NewValue, ESelectInfo::Type, int index);
	FText GetFieldItem(int index) const;
	void OnFieldSelectionChanged(TSharedPtr<FString> NewValue, ESelectInfo::Type, int index);
	FText GetOperatorItem(int index) const;
	void OnOperatorSelectionChanged(TSharedPtr<FString> NewValue, ESelectInfo::Type, int index);
	void UpdateClauseValue(const FText& text, int index);
	FReply AddClause(int index);
	FReply RemoveClause(int index);
	FReply SubmitQuery();
	FText GetButtonString() const;

	//Event filter Scroll box
	void GenerateScrollBox();
	FText GetTypeItem(int index) const;
	void OnTypeSelectionChanged(TSharedPtr<FString> NewValue, ESelectInfo::Type, int index);
	void OnEventChecked(ECheckBoxState NewState, FText eventName);
	FReply DisplayDrawColor(FText eventName, bool isHeatmap);
	void SelectDrawColor(FLinearColor NewColor);
	FReply SelectAll();
	FReply SelectNone();


	//Animation
	FReply PlayAnimation(int speed);
	void AnimationTimelineScrolled(float NewLevel);

	//Heatmap
	void OnHeatmapTypeSelectionChanged(TSharedPtr<FString> NewValue, ESelectInfo::Type);
	void OnHeatmapShapeTypeSelectionChanged(TSharedPtr<FString> NewValue, ESelectInfo::Type);
	void HeatmapSizeTextUpdate(const FText& InText, ETextCommit::Type InCommitType);
	void HeatmapMinValueTextUpdate(const FText& InText, ETextCommit::Type InCommitType);
	void HeatmapMaxValueTextUpdate(const FText& InText, ETextCommit::Type InCommitType);
	FText GetHeatmapTypeItem() const;
	FText GetHeatmapShapeTypeItem() const;
	FReply GenerateHeatmap();
	FReply GenerateHeatmap(SEventEditorContainer* collection, int first, int last);
	void SelectHeatmapDrawColor(FLinearColor NewColor);
	void HeatmapSizeScrolled(float NewLevel);
	void ResetRange();
	void OnHeatmapOrientationChecked(ECheckBoxState NewState);
	void OnHeatmapAnimationChecked(ECheckBoxState NewState);
	//void CreateHeatmapActor(UWorld* drawTarget, int index, FVector location, int value, int range, EventType shape, float size);
	//void CombineHeatmap();

	//Draw calls
	void DrawTelemetry(UWorld* drawTarget, TArray<SEventEditorContainer*> data);
	void DrawPoint(UWorld* drawTarget, TSharedPtr<STelemetryEvent> data, FColor color);
	void DrawPoints(UWorld* drawTarget, TArray<TSharedPtr<STelemetryEvent>> data, FColor color);
	void DrawPoints(UWorld* drawTarget, TArray<TSharedPtr<STelemetryEvent>> data, int start, int end, FColor color);
	void DrawSphere(UWorld* drawTarget, TSharedPtr<STelemetryEvent> data, FColor color);
	void DrawSpheres(UWorld* drawTarget, TArray<TSharedPtr<STelemetryEvent>> data, FColor color);
	void DrawSpheres(UWorld* drawTarget, TArray<TSharedPtr<STelemetryEvent>> data, int start, int end, FColor color);
	void DrawCube(UWorld* drawTarget, TSharedPtr<STelemetryEvent> data, FColor color);
	void DrawCubes(UWorld* drawTarget, TArray<TSharedPtr<STelemetryEvent>> data, FColor color);
	void DrawCubes(UWorld* drawTarget, TArray<TSharedPtr<STelemetryEvent>> data, int start, int end, FColor color);
	void CreateActor(UWorld* drawTarget, int index, TSharedPtr<STelemetryEvent> data, FColor color, EventType shape);
	void CreateActors(UWorld* drawTarget, TArray<TSharedPtr<STelemetryEvent>> data, FColor color, EventType shape);
	void CreateActors(UWorld* drawTarget, TArray<TSharedPtr<STelemetryEvent>> data, int start, int end, FColor color, EventType shape);
	void DestroyActors();
	void DestroyLastActor();

	//Draw state
	bool m_registeredWidget;
	bool m_drawChecked;
	bool m_needsActorUpdate;
	FText m_eventColor;

	//Query tools
	FQuerySerializer m_querySerializer;
	FQueryExecutor m_queryExecuter;
	QueryResultHandler m_queryResultHandler;
	bool m_isWaiting;

	//Animation state
	float m_anim_scrollBarLocation;
	AnimationController m_anim_Control;

	//Heatmap state
	EventType m_heatmapShapeType;
	HeatmapType m_heatmapType;
	HeatmapColors m_heatmapColor;
	UMaterial* m_material;
	float m_heatmapSize;
	int m_heatmapMinValue;
	int m_heatmapMaxValue;
	bool m_heatmapOrientation;
	bool m_heatmapAnimation;

	//Draw targets
	UWorld* m_editorDrawTarget;
	UWorld* m_PIEDrawTarget;
	UWorld* m_gameDrawTarget;

	//Window elements
	TSharedPtr<SEditableTextBox> m_searchText;
	TArray<TSharedPtr<SEditableTextBox>> m_valueText;
	TSharedPtr<SEditableTextBox> m_heatmapSizeText;
	TSharedPtr<SEditableTextBox> m_heatmapMinValueText;
	TSharedPtr<SEditableTextBox> m_heatmapMaxValueText;
	TSharedPtr<STextBlock> m_messageText;
	TSharedPtr<STextBlock> m_animationCurrentTime;
	TSharedPtr<STextBlock> m_animationTotalTime;
	TSharedPtr<SScrollBox> m_scrollBox;
	TSharedPtr<SScrollBox> m_queryScrollBox;
	TArray<TSharedPtr<FString>> m_eventgroupList;
	TArray<TSharedPtr<FString>> m_shapeList;
	TArray<TSharedPtr<FString>> m_heatmapTypeList;
	TArray<TSharedPtr<FString>> m_queryFieldList;
	TArray<TSharedPtr<FString>> m_queryOperatorList;
	TArray<TSharedPtr<FString>> m_queryAndOrList;
	TSharedPtr<FString> m_vizSelection;
	TSharedPtr<SSlider> m_animationSlider;
	TSharedPtr<SComboBox<TSharedPtr<FString>>> m_vizChoice;
	TSharedPtr<SBorder> m_heatmapLowColorButton;
	TSharedPtr<SBorder> m_heatmapHighColorButton;

	//Update timer
	FDelegateHandle m_tickDelegateHandle;
	FDelegateHandle m_startPIEDelegateHandle;
	FDelegateHandle m_endPIEDelegateHandle;

	//Editor flags/data
	void AddMenuExtensions(FMenuBuilder &);
	TSharedPtr<FUICommandList> m_pluginCommands;
	TSharedPtr<FExtensibilityManager> m_extensionManager;
	TSharedPtr< const FExtensionBase > m_menuExtension;
	TSharedPtr<FExtender> m_menuExtender;

	//All queried events
	TArray<SEventEditorContainer> m_queryEventCollection;
	TArray<SEventEditorContainer*> m_filterCollection;

	//Query settings
	TArray<SQuerySetting> m_queryCollection;

	//All drawn actors
	TArray<AActor*> m_eventActors;
};

