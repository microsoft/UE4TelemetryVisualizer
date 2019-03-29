//--------------------------------------------------------------------------------------
// TelemetryVisualizerDataTab.cpp
//
// Provides and manages data tab
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#pragma once

#include "TelemetryVisualizerUI.h"

#define LOCTEXT_NAMESPACE "Telemetry"

//Spawns the data tab layout
TSharedRef<SDockTab> FTelemetryVisualizerUI::SpawnDataTab(const FSpawnTabArgs& TabSpawnArgs)
{
	//Handler for query returns
	m_queryResultHandler.BindRaw(this, &FTelemetryVisualizerUI::QueryResults);
	m_isWaiting = false;

	TSharedRef<SDockTab> retTab = SNew(SDockTab)
		.Label(LOCTEXT("Data_Viewer", "Data Viewer"))
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.Padding(10.f, 5.f, 0.f, 0.f)
			[
				SNew(STextBlock)
					.Text(FText::FromString("Event Settings"))
			]
			+ SVerticalBox::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.Padding(10)
			[
				SNew(SBorder)
					.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
					.Content()
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						.Padding(10.f, 5.f, 0.f, 5.f)
					[
						SNew(STextBlock)
							.Text(FText::FromString("Create clauses to define what events are retrieved from the server. The local results are filtered in the search area"))
					]
					+ SVerticalBox::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						.Padding(10)
					[
						//Query controls are generated externally
						SAssignNew(m_queryScrollBox, SScrollBox)
							.Orientation(Orient_Vertical)
							.ScrollBarAlwaysVisible(true)
					]
					+ SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						.Padding(10)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
							.Padding(2.f, 0.f, 0.f, 0.f)
							.VAlign(VAlign_Center)
							.HAlign(HAlign_Fill)
						[
							SNew(SButton)
								.HAlign(HAlign_Center)
								.VAlign(VAlign_Center)
								.OnClicked_Raw(this, &FTelemetryVisualizerUI::AddClause, -1)
								.Text(LOCTEXT("Add_New_Clause", "Add New Clause"))
						]
						+ SHorizontalBox::Slot()
							.Padding(2.f, 0.f, 0.f, 0.f)
							.VAlign(VAlign_Center)
							.HAlign(HAlign_Fill)
						[
							SNew(SButton)
								.HAlign(HAlign_Center)
								.VAlign(VAlign_Center)
								.OnClicked_Raw(this, &FTelemetryVisualizerUI::SubmitQuery)
								.Text_Raw(this, &FTelemetryVisualizerUI::GetButtonString)
						]
					]
				]
			]
			+ SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.Padding(10.f, 5.f, 0.f, 0.f)
			[
				SNew(STextBlock)
					.Text(FText::FromString("Event Search"))
			]
			+ SVerticalBox::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.Padding(10)
			[
				SNew(SBorder)
					.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
					.Content()
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Top)
						.Padding(10)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
							.HAlign(HAlign_Fill)
							.VAlign(VAlign_Center)
						[
							SAssignNew(m_searchText, SEditableTextBox)
								.Text(LOCTEXT("Search_Terms", ""))
						]
						+ SHorizontalBox::Slot()
							.AutoWidth()
							.Padding(2.f, 0.f, 0.f, 0.f)
							.VAlign(VAlign_Center)
						[
							SNew(SButton)
								.OnClicked_Raw(this, &FTelemetryVisualizerUI::UpdateFilterEvents)
								.ButtonStyle(FEditorStyle::Get(), "HoverHintOnly")
							[
								SNew(SBox)
									.WidthOverride(MultiBoxConstants::MenuIconSize)
									.HeightOverride(MultiBoxConstants::MenuIconSize)
								[
									SNew(SImage)
										.Image(FSlateIcon(FEditorStyle::GetStyleSetName(), "Symbols.SearchGlass").GetIcon())
								]
							]
						]
					]
					+ SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						.Padding(10, 0, 0, 0)
					[
						SAssignNew(m_messageText, STextBlock)
					]
					+ SVerticalBox::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						.Padding(10)
					[
						//Event filters are generated externally
						SAssignNew(m_scrollBox, SScrollBox)
							.Orientation(Orient_Vertical)
							.ScrollBarAlwaysVisible(true)
					]
					+ SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						.Padding(10)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
							.Padding(2.f, 0.f, 0.f, 0.f)
							.VAlign(VAlign_Center)
							.HAlign(HAlign_Fill)
						[
							SNew(SButton)
								.VAlign(VAlign_Center)
								.HAlign(HAlign_Center)
								.Text(LOCTEXT("Select_All", "Select All"))
								.OnClicked_Raw(this, &FTelemetryVisualizerUI::SelectAll)
						]
						+ SHorizontalBox::Slot()
							.Padding(2.f, 0.f, 0.f, 0.f)
							.VAlign(VAlign_Center)
							.HAlign(HAlign_Fill)
						[
							SNew(SButton)
								.VAlign(VAlign_Center)
								.HAlign(HAlign_Center)
								.Text(LOCTEXT("Select_None", "Select None"))
								.OnClicked_Raw(this, &FTelemetryVisualizerUI::SelectNone)
						]
					]
				]
			]
		];

	GenerateQueryScrollBox();

	return retTab;
}

/////////////////////////////////QUERY SCROLLBOX/////////////////////////////////
//Generates the list query conditions
void FTelemetryVisualizerUI::GenerateQueryScrollBox()
{
	if (!m_queryScrollBox.IsValid())
	{
		return;
	}
	m_queryScrollBox->ClearChildren();

	m_queryScrollBox->AddSlot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(2.f, 0.f, 0.f, 0.f)
			.VAlign(VAlign_Center)
		[
			// +
			SNew(SButton)
				.OnClicked_Raw(this, &FTelemetryVisualizerUI::AddClause, 0)
				.ButtonStyle(FEditorStyle::Get(), "HoverHintOnly")
			[
				SNew(SBox)
					.WidthOverride(MultiBoxConstants::MenuIconSize)
					.HeightOverride(MultiBoxConstants::MenuIconSize)
				[
					SNew(SImage)
						.Image(FSlateIcon(FEditorStyle::GetStyleSetName(), "WorldBrowser.AddLayer").GetIcon())
				]
			]
		]
		+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(54.f, 0.f, 0.f, 0.f)
			.VAlign(VAlign_Center)
		[
			//FIELD
			SNew(SBox)
				.WidthOverride(90)
			[
				SNew(SComboBox<TSharedPtr<FString>>)
					.OptionsSource(&m_queryFieldList)
					.OnSelectionChanged_Raw(this, &FTelemetryVisualizerUI::OnFieldSelectionChanged, 0)
					.OnGenerateWidget_Raw(this, &FTelemetryVisualizerUI::MakeWidgetForOption)
					.InitiallySelectedItem(m_queryFieldList[m_queryCollection[0].Field])
				[
					SNew(STextBlock)
						.Text_Raw(this, &FTelemetryVisualizerUI::GetFieldItem, 0)
				]
			]
		]
		+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(2.f, 0.f, 0.f, 0.f)
			.VAlign(VAlign_Center)
		[
			//OPERATOR
			SNew(SBox)
				.WidthOverride(40)
			[
				SNew(SComboBox<TSharedPtr<FString>>)
					.OptionsSource(&m_queryOperatorList)
					.OnSelectionChanged_Raw(this, &FTelemetryVisualizerUI::OnOperatorSelectionChanged, 0)
					.OnGenerateWidget_Raw(this, &FTelemetryVisualizerUI::MakeWidgetForOption)
					.InitiallySelectedItem(m_queryOperatorList[m_queryCollection[0].Operator])
				[
					SNew(STextBlock)
						.Text_Raw(this, &FTelemetryVisualizerUI::GetOperatorItem, 0)
				]
			]
		]
		+ SHorizontalBox::Slot()
			.HAlign(HAlign_Fill)
			.Padding(2.f, 0.f, 26.f, 0.f)
			.VAlign(VAlign_Center)
		[
			//VALUE
			SAssignNew(m_valueText[0], SEditableTextBox)
				.OnTextChanged_Raw(this, &FTelemetryVisualizerUI::UpdateClauseValue, 0)
				.Text(FText::FromString(m_queryCollection[0].Value))
		]
	];

	for (int i = 1; i < m_queryCollection.Num(); i++)
	{
		m_queryScrollBox->AddSlot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(2.f, 0.f, 0.f, 0.f)
				.VAlign(VAlign_Center)
			[
				// +
				SNew(SButton)
					.OnClicked_Raw(this, &FTelemetryVisualizerUI::AddClause, i)
					.ButtonStyle(FEditorStyle::Get(), "HoverHintOnly")
				[
					SNew(SBox)
						.WidthOverride(MultiBoxConstants::MenuIconSize)
						.HeightOverride(MultiBoxConstants::MenuIconSize)
					[
						SNew(SImage)
							.Image(FSlateIcon(FEditorStyle::GetStyleSetName(), "WorldBrowser.AddLayer").GetIcon())
					]
				]
			]
			+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(2.f, 0.f, 0.f, 0.f)
				.VAlign(VAlign_Center)
			[
				//AND / OR
				SNew(SBox)
					.WidthOverride(50)
				[
					SNew(SComboBox<TSharedPtr<FString>>)
						.OptionsSource(&m_queryAndOrList)
						.OnSelectionChanged_Raw(this, &FTelemetryVisualizerUI::OnAndOrSelectionChanged, i)
						.OnGenerateWidget_Raw(this, &FTelemetryVisualizerUI::MakeWidgetForOption)
						.InitiallySelectedItem(m_queryAndOrList[m_queryCollection[i].isAnd])
					[
						SNew(STextBlock)
							.Text_Raw(this, &FTelemetryVisualizerUI::GetAndOrItem, i)
					]
				]
			]
			+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(2.f, 0.f, 0.f, 0.f)
				.VAlign(VAlign_Center)
			[
				//FIELD
				SNew(SBox)
					.WidthOverride(90)
				[
					SNew(SComboBox<TSharedPtr<FString>>)
						.OptionsSource(&m_queryFieldList)
						.OnSelectionChanged_Raw(this, &FTelemetryVisualizerUI::OnFieldSelectionChanged, i)
						.OnGenerateWidget_Raw(this, &FTelemetryVisualizerUI::MakeWidgetForOption)
						.InitiallySelectedItem(m_queryFieldList[m_queryCollection[i].Field])
					[
						SNew(STextBlock)
							.Text_Raw(this, &FTelemetryVisualizerUI::GetFieldItem, i)
					]
				]
			]
			+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(2.f, 0.f, 0.f, 0.f)
				.VAlign(VAlign_Center)
			[
				//OPERATOR
				SNew(SBox)
					.WidthOverride(40)
				[
					SNew(SComboBox<TSharedPtr<FString>>)
						.OptionsSource(&m_queryOperatorList)
						.OnSelectionChanged_Raw(this, &FTelemetryVisualizerUI::OnOperatorSelectionChanged, i)
						.OnGenerateWidget_Raw(this, &FTelemetryVisualizerUI::MakeWidgetForOption)
						.InitiallySelectedItem(m_queryOperatorList[m_queryCollection[i].Operator])
					[
						SNew(STextBlock)
							.Text_Raw(this, &FTelemetryVisualizerUI::GetOperatorItem, i)
					]
				]
			]
			+ SHorizontalBox::Slot()
				.HAlign(HAlign_Fill)
				.Padding(2.f, 0.f, 0.f, 0.f)
				.VAlign(VAlign_Center)
			[
				//VALUE
				SAssignNew(m_valueText[i], SEditableTextBox)
					.OnTextChanged_Raw(this, &FTelemetryVisualizerUI::UpdateClauseValue, i)
					.Text(FText::FromString(m_queryCollection[i].Value))
			]
			+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(2.f, 0.f, 0.f, 0.f)
				.VAlign(VAlign_Center)
			[
				// X
				SNew(SButton)
					.OnClicked_Raw(this, &FTelemetryVisualizerUI::RemoveClause, i)
					.ButtonStyle(FEditorStyle::Get(), "HoverHintOnly")
				[
					SNew(SBox)
						.WidthOverride(MultiBoxConstants::MenuIconSize)
						.HeightOverride(MultiBoxConstants::MenuIconSize)
					[
						SNew(SImage)
							.Image(FSlateIcon(FEditorStyle::GetStyleSetName(), "Symbols.X").GetIcon())
					]
				]
			]
		];
	}
}

FReply FTelemetryVisualizerUI::AddClause(int index)
{
	if (index == -1)
	{
		m_queryCollection.Add(SQuerySetting());
		m_valueText.SetNum(m_valueText.Num() + 1);
	}
	else
	{
		m_queryCollection.Insert(SQuerySetting(), index);
		m_valueText.InsertDefaulted(index, 1);
	}

	GenerateQueryScrollBox();

	return FReply::Handled();
}

FReply FTelemetryVisualizerUI::RemoveClause(int index)
{
	m_queryCollection.RemoveAt(index);
	m_valueText.RemoveAt(index);
	GenerateQueryScrollBox();

	return FReply::Handled();
}

FText FTelemetryVisualizerUI::GetButtonString() const
{
	if (m_isWaiting)
	{
		return LOCTEXT("Running...", "Running...");
	}
	return LOCTEXT("Submit", "Submit");
}

//Translates query UI in to query nodes for execution
FReply FTelemetryVisualizerUI::SubmitQuery()
{
	FQueryNodeList andNodes;
	FQueryNodeList orNodes;

	m_queryCollection[0].isAnd = true;

	for (int i = 0; i < m_queryCollection.Num(); i++)
	{
		EQueryOp op = EQueryOp::Eq;
		switch (m_queryCollection[i].Operator)
		{
		case QueryOperator::Not_Equal:
			op = EQueryOp::Neq;
			break;
		case QueryOperator::GreaterThan:
			op = EQueryOp::Gt;
			break;
		case QueryOperator::LessThan:
			op = EQueryOp::Lt;
			break;
		case QueryOperator::GreaterThanOrEqual:
			op = EQueryOp::Gte;
			break;
		case QueryOperator::LessThanOrEqual:
			op = EQueryOp::Lte;
			break;
		}

		if (m_queryCollection[i].isAnd)
		{
			andNodes.Emplace(new FQueryNode(EQueryNodeType::Comparison, QueryExpectedStrings[m_queryCollection[i].Field], op, FJsonValueUtil::Create(m_queryCollection[i].Value)));
		}
		else
		{
			orNodes.Emplace(new FQueryNode(EQueryNodeType::Comparison, QueryExpectedStrings[m_queryCollection[i].Field], op, FJsonValueUtil::Create(m_queryCollection[i].Value)));
		}
	}

	if (m_queryCollection.Num() == 1)
	{
		CollectEvents(andNodes[0]);
	}
	else
	{
		if (orNodes.Num() == 1)
		{
			andNodes.Emplace(orNodes[0]);
			CollectEvents(MakeShareable(new FQueryNode(EQueryNodeType::Group, EQueryOp::Or, MoveTemp(andNodes))));
		}
		else
		{
			if (orNodes.Num() > 1)
			{
				andNodes.Emplace(new FQueryNode(EQueryNodeType::Group, EQueryOp::Or, MoveTemp(orNodes)));
			}

			CollectEvents(MakeShareable(new FQueryNode(EQueryNodeType::Group, EQueryOp::And, MoveTemp(andNodes))));
		}
	}

	return FReply::Handled();
}

FText FTelemetryVisualizerUI::GetAndOrItem(int index) const
{
	FString typeString = "Or";

	if (m_queryCollection[index].isAnd)
	{
		typeString = "And";
	}

	return FText::FromString(*typeString);
}

void FTelemetryVisualizerUI::OnAndOrSelectionChanged(TSharedPtr<FString> NewValue, ESelectInfo::Type, int index)
{
	if (NewValue.IsValid())
	{
		bool isAnd = false;

		if (*NewValue == "And")
		{
			isAnd = true;
		}

		if (m_queryCollection[index].isAnd != isAnd)
		{
			m_queryCollection[index].isAnd = isAnd;
		}
	}
}

FText FTelemetryVisualizerUI::GetFieldItem(int index) const
{
	return FText::FromString(*QueryFieldStrings[m_queryCollection[index].Field]);
	return FText::FromString(*QueryOperatorStrings[m_queryCollection[index].Operator]);
}

void FTelemetryVisualizerUI::OnFieldSelectionChanged(TSharedPtr<FString> NewValue, ESelectInfo::Type, int index)
{
	if (NewValue.IsValid())
	{
		QueryField newType = QueryField::Category;

		for (int i = 1; i < QueryFieldStrings.Num(); i++)
		{
			if (*NewValue == QueryFieldStrings[i])
			{
				newType = (QueryField)i;
			}
		}

		if (m_queryCollection[index].Field != newType)
		{
			m_queryCollection[index].Field = newType;
		}
	}
}

FText FTelemetryVisualizerUI::GetOperatorItem(int index) const
{
	return FText::FromString(*QueryOperatorStrings[m_queryCollection[index].Operator]);
}

void FTelemetryVisualizerUI::OnOperatorSelectionChanged(TSharedPtr<FString> NewValue, ESelectInfo::Type, int index)
{
	if (NewValue.IsValid())
	{
		QueryOperator newType = QueryOperator::Equal;

		for (int i = 1; i < QueryOperatorStrings.Num(); i++)
		{
			if (*NewValue == QueryOperatorStrings[i])
			{
				newType = (QueryOperator)i;
			}
		}

		if (m_queryCollection[index].Operator != newType)
		{
			m_queryCollection[index].Operator = newType;
		}
	}
}

void FTelemetryVisualizerUI::UpdateClauseValue(const FText& text, int index)
{
	m_queryCollection[index].Value = text.ToString();
	return;
}

//Call to execute query
void FTelemetryVisualizerUI::CollectEvents(FQueryNodePtr query)
{
	if (!m_isWaiting)
	{
		m_isWaiting = true;
		AsyncTask(ENamedThreads::GameThread, [this, query]()
		{
			m_queryExecuter.ExecuteCustomQuery(m_querySerializer.Serialize(query), m_queryResultHandler);
		});
	}
}

//Called when the query request completes.  Converts results to a local collection
void FTelemetryVisualizerUI::QueryResults(TSharedPtr<SQueryResult> results)
{
	m_queryEventCollection.Empty();

	FString currentName;

	for (auto newEvent : results->Events)
	{
		int currentIdx = -1;
		currentName = newEvent.GetName();

		for (int i = 0; i < m_queryEventCollection.Num(); i++)
		{
			if (m_queryEventCollection[i].name == currentName)
			{
				currentIdx = i;
				break;
			}
		}

		if (currentIdx == -1)
		{
			currentIdx = m_queryEventCollection.Emplace(SEventEditorContainer(currentName, m_queryEventCollection.Num()));
		}

		m_queryEventCollection[currentIdx].AddEvent(newEvent);
	}

	int count = FilterEvents();
	GenerateScrollBoxes(count);
	m_isWaiting = false;
}

/////////////////////////////////EVENT COLLECTION/////////////////////////////////
//Generates the list of available and filtered events for the tab
void FTelemetryVisualizerUI::GenerateScrollBox()
{
	if (!m_scrollBox.IsValid())
	{
		return;
	}
	m_scrollBox->ClearChildren();

	for (int i = 0; i < m_filterCollection.Num(); i++)
	{
		m_scrollBox->AddSlot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Center)
				[
					SNew(SCheckBox)
						.IsChecked_Lambda([this, i]() -> ECheckBoxState
						{
							if (m_filterCollection[i]->ShouldDraw()) return ECheckBoxState::Checked;
							return ECheckBoxState::Unchecked;
						})
						.OnCheckStateChanged_Raw(this, &FTelemetryVisualizerUI::OnEventChecked, FText::FromString(m_filterCollection[i]->name))
					[
						SNew(STextBlock)
							.Text(FText::FromString(m_filterCollection[i]->name))
					]
				]
				+ SHorizontalBox::Slot()
					.AutoWidth()
					.HAlign(HAlign_Right)
					.VAlign(VAlign_Center)
				[
					SNew(SComboBox<TSharedPtr<FString>>)
						.OptionsSource(&m_shapeList)
						.OnSelectionChanged_Raw(this, &FTelemetryVisualizerUI::OnTypeSelectionChanged, i)
						.OnGenerateWidget_Raw(this, &FTelemetryVisualizerUI::MakeWidgetForOption)
						.InitiallySelectedItem(m_shapeList[0])
					[
						SNew(STextBlock)
							.Text_Raw(this, &FTelemetryVisualizerUI::GetTypeItem, i)
					]
				]
				+ SHorizontalBox::Slot()
					.AutoWidth()
					.HAlign(HAlign_Right)
					.VAlign(VAlign_Center)
				[
					SNew(SButton)
						.OnClicked_Raw(this, &FTelemetryVisualizerUI::DisplayDrawColor, FText::FromString(m_filterCollection[i]->name), false)
					[
						SNew(SBorder)
							.Padding(FMargin(6))
							.BorderBackgroundColor(FSlateColor(m_filterCollection[i]->GetColor()))
							.BorderImage(m_filterCollection[i]->GetBrush())
					]
				]
			];
	}
}

FReply FTelemetryVisualizerUI::UpdateFilterEvents()
{
	int count = FilterEvents();
	GenerateScrollBoxes(count);

	return FReply::Handled();
}

int FTelemetryVisualizerUI::FilterEvents()
{
	int count = 0;
	FString searchText = m_searchText.IsValid() ? m_searchText->GetText().ToString() : "";
	m_filterCollection.Empty();

	if (searchText == "")
	{
		for (int i = 0; i < m_queryEventCollection.Num(); i++)
		{
			m_filterCollection.Emplace(&m_queryEventCollection[i]);
			count += m_queryEventCollection[i].events.Num();
		}
	}
	else
	{
		for (int i = 0; i < m_queryEventCollection.Num(); i++)
		{
			if (m_queryEventCollection[i].name.Contains(searchText))
			{
				m_filterCollection.Emplace(&m_queryEventCollection[i]);
				count += m_queryEventCollection[i].events.Num();
			}
		}
	}

	return count;
}

void FTelemetryVisualizerUI::GenerateScrollBoxes(int count)
{
	GenerateScrollBox();
	GenerateEventBox();
	m_needsActorUpdate = true;

	if (m_messageText.IsValid())
	{
		m_messageText->SetText(FText::Format(LOCTEXT("Event_Count", "Found {0} events"), FText::AsNumber(count)));
	}
}

/////////////////////////////////EVENT FILTER SCROLLBOX/////////////////////////////////
void FTelemetryVisualizerUI::OnEventChecked(ECheckBoxState NewState, FText eventName)
{
	for (int i = 0; i < m_filterCollection.Num(); i++)
	{
		if (m_filterCollection[i]->name == eventName.ToString())
		{
			if (NewState == ECheckBoxState::Checked)
			{
				m_filterCollection[i]->SetShouldDraw(true);
			}
			else
			{
				m_filterCollection[i]->SetShouldDraw(false);
			}

			m_needsActorUpdate = true;
			break;
		}
	}
}

FReply FTelemetryVisualizerUI::DisplayDrawColor(FText eventName, bool isHeatmap)
{
	FColorPickerArgs PickerArgs;
	PickerArgs.DisplayGamma = TAttribute<float>::Create(TAttribute<float>::FGetter::CreateUObject(GEngine, &UEngine::GetDisplayGamma));

	if (isHeatmap)
	{
		PickerArgs.bUseAlpha = true;
		PickerArgs.OnColorCommitted = FOnLinearColorValueChanged::CreateRaw(this, &FTelemetryVisualizerUI::SelectHeatmapDrawColor);

		if (eventName.ToString() == "High")
		{
			PickerArgs.InitialColorOverride = FLinearColor(m_heatmapColor.GetHighColor());
		}
		else
		{
			PickerArgs.InitialColorOverride = FLinearColor(m_heatmapColor.GetLowColor());
		}
	}
	else
	{
		PickerArgs.bUseAlpha = false;
		PickerArgs.OnColorCommitted = FOnLinearColorValueChanged::CreateRaw(this, &FTelemetryVisualizerUI::SelectDrawColor);

		for (int i = 0; i < m_filterCollection.Num(); i++)
		{
			if (m_filterCollection[i]->name == eventName.ToString())
			{
				PickerArgs.InitialColorOverride = FLinearColor(m_filterCollection[i]->GetColor());
				break;
			}
		}
	}

	m_eventColor = eventName;

	OpenColorPicker(PickerArgs);

	return FReply::Handled();
}

void FTelemetryVisualizerUI::SelectDrawColor(FLinearColor NewColor)
{
	for (int i = 0; i < m_filterCollection.Num(); i++)
	{
		if (m_filterCollection[i]->name == m_eventColor.ToString())
		{
			m_filterCollection[i]->SetColor(NewColor.Quantize());
			m_needsActorUpdate = true;
			break;
		}
	}

	GenerateScrollBox();
}

FText FTelemetryVisualizerUI::GetTypeItem(int index) const
{
	FString typeString = "Sphere";

	SEventEditorContainer* temp = m_filterCollection[index];
	EventType currentType = temp->GetShapeType();

	switch (currentType)
	{
	case EventType::Cone:
		typeString = "Cone";
		break;
	case EventType::Cube:
		typeString = "Cube";
		break;
	case EventType::Cylinder:
		typeString = "Cylinder";
		break;
	case EventType::Plane:
		typeString = "Plane";
		break;
	}

	return FText::FromString(*typeString);
}

void FTelemetryVisualizerUI::OnTypeSelectionChanged(TSharedPtr<FString> NewValue, ESelectInfo::Type, int index)
{
	if (NewValue.IsValid())
	{
		EventType newType = EventType::Sphere;

		if (*NewValue == "Cone")
		{
			newType = EventType::Cone;
		}
		else if (*NewValue == "Cube")
		{
			newType = EventType::Cube;
		}
		else if (*NewValue == "Cylinder")
		{
			newType = EventType::Cylinder;
		}
		else if (*NewValue == "Plane")
		{
			newType = EventType::Plane;
		}

		if (m_filterCollection[index]->GetShapeType() != newType)
		{
			m_filterCollection[index]->SetShapeType(newType);
			m_needsActorUpdate = true;
		}
	}
}

void FTelemetryVisualizerUI::GenerateEventBox()
{
	m_eventgroupList.Empty();
	m_eventgroupList.Add(MakeShareable<FString>(new FString("")));
	m_vizSelection = m_eventgroupList[0];

	for (int i = 0; i < m_filterCollection.Num(); i++)
	{
		m_eventgroupList.Add(MakeShareable<FString>(new FString(m_filterCollection[i]->name)));
	}

	if (m_vizChoice.IsValid())
	{
		m_vizChoice->SetSelectedItem(m_eventgroupList[0]);
		m_vizChoice->RefreshOptions();
	}
}

FReply FTelemetryVisualizerUI::SelectAll()
{
	for (int i = 0; i < m_filterCollection.Num(); i++)
	{
		m_filterCollection[i]->SetShouldDraw(true);
		m_needsActorUpdate = true;
	}

	return FReply::Handled();
}

FReply FTelemetryVisualizerUI::SelectNone()
{
	for (int i = 0; i < m_filterCollection.Num(); i++)
	{
		m_filterCollection[i]->SetShouldDraw(false);
		m_needsActorUpdate = true;
	}

	return FReply::Handled();
}
