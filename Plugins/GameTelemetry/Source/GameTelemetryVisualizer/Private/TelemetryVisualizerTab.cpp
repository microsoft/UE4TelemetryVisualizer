//--------------------------------------------------------------------------------------
// TelemetryVisualizerTab.cpp
//
// Provides and manages visual tools tab
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#pragma once

#include "TelemetryVisualizerUI.h"

#define LOCTEXT_NAMESPACE "Telemetry"

//Spawns the visual tools tab layout
TSharedRef<SDockTab> FTelemetryVisualizerUI::SpawnVizTab(const FSpawnTabArgs& TabSpawnArgs)
{
	TSharedRef<SDockTab> retTab = SNew(SDockTab)
		.Label(LOCTEXT("Visualization_Tools", "Visualization Tools"))
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
					.Text(FText::FromString("Event Type"))
			]
			+ SVerticalBox::Slot()
				.AutoHeight()
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
						.Padding(40.f, 10.f, 40.f, 10.f)
					[
						//Event to act on
						SAssignNew(m_vizChoice, SComboBox<TSharedPtr<FString>>)
							.OptionsSource(&m_eventgroupList)
							.OnSelectionChanged_Raw(this, &FTelemetryVisualizerUI::OnVizSelectionChanged)
							.OnGenerateWidget_Raw(this, &FTelemetryVisualizerUI::MakeWidgetForOption)
							.InitiallySelectedItem(m_vizSelection)
						[
							SNew(STextBlock)
								.Text_Raw(this, &FTelemetryVisualizerUI::GetCurrentVizItem)
						]
					]
					//Animation Controls - Start
					+ SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						.Padding(40.f, 10.f, 40.f, 10.f)
					[
						SAssignNew(m_animationSlider, SSlider)
							.Value(0.f)
							.OnValueChanged_Raw(this, &FTelemetryVisualizerUI::AnimationTimelineScrolled)
					]
					+ SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						.Padding(40.f, 0.f, 40.f, 10.f)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
							.Padding(2.f, 0.f, 0.f, 0.f)
							.VAlign(VAlign_Center)
							.HAlign(HAlign_Left)
						[
							SAssignNew(m_animationCurrentTime, STextBlock)
						]
						+ SHorizontalBox::Slot()
							.Padding(2.f, 0.f, 0.f, 0.f)
							.VAlign(VAlign_Center)
							.HAlign(HAlign_Right)
						[
							SAssignNew(m_animationTotalTime, STextBlock)
						]
					]
					+ SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						.Padding(40.f, 10.f, 40.f, 10.f)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
							.Padding(2.f, 0.f, 0.f, 0.f)
							.VAlign(VAlign_Center)
							.HAlign(HAlign_Fill)
						[
							//Triple reverse button
							SNew(SButton)
								.VAlign(VAlign_Center)
								.HAlign(HAlign_Center)
								.OnClicked_Raw(this, &FTelemetryVisualizerUI::PlayAnimation, -4)
								.ButtonStyle(FEditorStyle::Get(), "FlatButton.Default")
							[
								SNew(SHorizontalBox)
								+ SHorizontalBox::Slot()
									.VAlign(VAlign_Center)
									.HAlign(HAlign_Fill)
								[
									SNew(SBox)
										.WidthOverride(MultiBoxConstants::MenuIconSize)
										.HeightOverride(MultiBoxConstants::MenuIconSize)
									[
										SNew(SImage)
											.Image(FSlateIcon(FEditorStyle::GetStyleSetName(), "GenericPlay").GetIcon())
											.RenderTransform(FSlateRenderTransform(FQuat2D(PI), FVector2D(16.f, 16.f)))
									]
								]
								+ SHorizontalBox::Slot()
									.VAlign(VAlign_Center)
									.HAlign(HAlign_Fill)
								[
									SNew(SBox)
										.WidthOverride(MultiBoxConstants::MenuIconSize)
										.HeightOverride(MultiBoxConstants::MenuIconSize)
									[
										SNew(SImage)
											.Image(FSlateIcon(FEditorStyle::GetStyleSetName(), "GenericPlay").GetIcon())
											.RenderTransform(FSlateRenderTransform(FQuat2D(PI), FVector2D(16.f, 16.f)))
									]
								]
								+ SHorizontalBox::Slot()
									.VAlign(VAlign_Center)
									.HAlign(HAlign_Fill)
								[
									SNew(SBox)
										.WidthOverride(MultiBoxConstants::MenuIconSize)
										.HeightOverride(MultiBoxConstants::MenuIconSize)
									[
										SNew(SImage)
											.Image(FSlateIcon(FEditorStyle::GetStyleSetName(), "GenericPlay").GetIcon())
											.RenderTransform(FSlateRenderTransform(FQuat2D(PI), FVector2D(16.f, 16.f)))
									]
								]
							]
						]
						+ SHorizontalBox::Slot()
							.Padding(2.f, 0.f, 0.f, 0.f)
							.VAlign(VAlign_Center)
							.HAlign(HAlign_Fill)
						[
							//Double reverse button
							SNew(SButton)
								.VAlign(VAlign_Center)
								.HAlign(HAlign_Center)
								.OnClicked_Raw(this, &FTelemetryVisualizerUI::PlayAnimation, -2)
								.ButtonStyle(FEditorStyle::Get(), "FlatButton.Default")
							[
								SNew(SHorizontalBox)
								+ SHorizontalBox::Slot()
									.VAlign(VAlign_Center)
									.HAlign(HAlign_Fill)
								[
									SNew(SBox)
										.WidthOverride(MultiBoxConstants::MenuIconSize)
										.HeightOverride(MultiBoxConstants::MenuIconSize)
									[
										SNew(SImage)
											.Image(FSlateIcon(FEditorStyle::GetStyleSetName(), "GenericPlay").GetIcon())
											.RenderTransform(FSlateRenderTransform(FQuat2D(PI), FVector2D(16.f, 16.f)))
									]
								]
								+ SHorizontalBox::Slot()
									.VAlign(VAlign_Center)
									.HAlign(HAlign_Fill)
								[
									SNew(SBox)
										.WidthOverride(MultiBoxConstants::MenuIconSize)
										.HeightOverride(MultiBoxConstants::MenuIconSize)
									[
										SNew(SImage)
											.Image(FSlateIcon(FEditorStyle::GetStyleSetName(), "GenericPlay").GetIcon())
											.RenderTransform(FSlateRenderTransform(FQuat2D(PI), FVector2D(16.f, 16.f)))
									]
								]
							]
						]
						+ SHorizontalBox::Slot()
							.Padding(2.f, 0.f, 0.f, 0.f)
							.VAlign(VAlign_Center)
							.HAlign(HAlign_Fill)
						[
							//Pause button
							SNew(SButton)
								.VAlign(VAlign_Center)
								.HAlign(HAlign_Center)
								.OnClicked_Raw(this, &FTelemetryVisualizerUI::PlayAnimation, 0)
								.ButtonStyle(FEditorStyle::Get(), "FlatButton.Default")
							[
								SNew(SBox)
									.WidthOverride(MultiBoxConstants::MenuIconSize)
									.HeightOverride(MultiBoxConstants::MenuIconSize)
								[
									SNew(SImage)
										.Image(FSlateIcon(FEditorStyle::GetStyleSetName(), "GenericPause").GetIcon())
								]
							]
						]
						+ SHorizontalBox::Slot()
							.Padding(2.f, 0.f, 0.f, 0.f)
							.VAlign(VAlign_Center)
							.HAlign(HAlign_Fill)
						[
							//Play button
							SNew(SButton)
								.VAlign(VAlign_Center)
								.HAlign(HAlign_Center)
								.OnClicked_Raw(this, &FTelemetryVisualizerUI::PlayAnimation, 1)
								.ButtonStyle(FEditorStyle::Get(), "FlatButton.Default")
							[
								SNew(SBox)
									.WidthOverride(MultiBoxConstants::MenuIconSize)
									.HeightOverride(MultiBoxConstants::MenuIconSize)
								[
									SNew(SImage)
										.Image(FSlateIcon(FEditorStyle::GetStyleSetName(), "GenericPlay").GetIcon())
								]
							]
						]
						+ SHorizontalBox::Slot()
							.Padding(2.f, 0.f, 0.f, 0.f)
							.VAlign(VAlign_Center)
							.HAlign(HAlign_Fill)
						[
							//Double fast forward button
							SNew(SButton)
								.VAlign(VAlign_Center)
								.HAlign(HAlign_Center)
								.OnClicked_Raw(this, &FTelemetryVisualizerUI::PlayAnimation, 2)
								.ButtonStyle(FEditorStyle::Get(), "FlatButton.Default")
							[
								SNew(SHorizontalBox)
								+ SHorizontalBox::Slot()
									.VAlign(VAlign_Center)
									.HAlign(HAlign_Fill)
								[
									SNew(SBox)
										.WidthOverride(MultiBoxConstants::MenuIconSize)
										.HeightOverride(MultiBoxConstants::MenuIconSize)
									[
										SNew(SImage)
											.Image(FSlateIcon(FEditorStyle::GetStyleSetName(), "GenericPlay").GetIcon())
									]
								]
								+ SHorizontalBox::Slot()
									.VAlign(VAlign_Center)
									.HAlign(HAlign_Fill)
								[
									SNew(SBox)
										.WidthOverride(MultiBoxConstants::MenuIconSize)
										.HeightOverride(MultiBoxConstants::MenuIconSize)
									[
										SNew(SImage)
											.Image(FSlateIcon(FEditorStyle::GetStyleSetName(), "GenericPlay").GetIcon())
									]
								]
							]
						]
						+ SHorizontalBox::Slot()
							.Padding(2.f, 0.f, 0.f, 0.f)
							.VAlign(VAlign_Center)
							.HAlign(HAlign_Fill)
						[
							//Triple fast forward button
							SNew(SButton)
								.VAlign(VAlign_Center)
								.HAlign(HAlign_Center)
								.OnClicked_Raw(this, &FTelemetryVisualizerUI::PlayAnimation, 4)
								.ButtonStyle(FEditorStyle::Get(), "FlatButton.Default")
							[
								SNew(SHorizontalBox)
								+ SHorizontalBox::Slot()
									.VAlign(VAlign_Center)
									.HAlign(HAlign_Fill)
								[
									SNew(SBox)
										.WidthOverride(MultiBoxConstants::MenuIconSize)
										.HeightOverride(MultiBoxConstants::MenuIconSize)
									[
										SNew(SImage)
											.Image(FSlateIcon(FEditorStyle::GetStyleSetName(), "GenericPlay").GetIcon())
									]
								]
								+ SHorizontalBox::Slot()
									.VAlign(VAlign_Center)
									.HAlign(HAlign_Fill)
								[
									SNew(SBox)
										.WidthOverride(MultiBoxConstants::MenuIconSize)
										.HeightOverride(MultiBoxConstants::MenuIconSize)
									[
										SNew(SImage)
											.Image(FSlateIcon(FEditorStyle::GetStyleSetName(), "GenericPlay").GetIcon())
									]
								]
								+ SHorizontalBox::Slot()
									.VAlign(VAlign_Center)
									.HAlign(HAlign_Fill)
								[
									SNew(SBox)
										.WidthOverride(MultiBoxConstants::MenuIconSize)
										.HeightOverride(MultiBoxConstants::MenuIconSize)
									[
										SNew(SImage)
											.Image(FSlateIcon(FEditorStyle::GetStyleSetName(), "GenericPlay").GetIcon())
									]
								]
							]
						]
					]
					//Animation Controls - End
				]
			]
			+ SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.Padding(10.f, 5.f, 0.f, 0.f)
			[
				SNew(STextBlock)
					.Text(FText::FromString("Heatmap Settings"))
			]
			+ SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.Padding(10)
			[
				SNew(SBorder)
					.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
					.Content()
				[
					//Heatmap Controls - Start
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						.Padding(40.f, 10.f, 40.f, 5.f)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
							.AutoWidth()
							.Padding(2.f, 0.f, 0.f, 0.f)
							.VAlign(VAlign_Center)
							.HAlign(HAlign_Fill)
						[
							SNew(SBox)
								.WidthOverride(90.f)
							[
								SNew(STextBlock).Text(LOCTEXT("Type", "Type"))
							]
						]
						+ SHorizontalBox::Slot()
							.Padding(2.f, 0.f, 0.f, 0.f)
							.VAlign(VAlign_Center)
							.HAlign(HAlign_Fill)
						[
							SNew(SComboBox<TSharedPtr<FString>>)
								.OptionsSource(&m_heatmapTypeList)
								.OnSelectionChanged_Raw(this, &FTelemetryVisualizerUI::OnHeatmapTypeSelectionChanged)
								.OnGenerateWidget_Raw(this, &FTelemetryVisualizerUI::MakeWidgetForOption)
								.InitiallySelectedItem(m_heatmapTypeList[0])
							[
								SNew(STextBlock)
									.Text_Raw(this, &FTelemetryVisualizerUI::GetHeatmapTypeItem)
							]
						]
					]
					+ SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						.Padding(40.f, 5.f, 40.f, 5.f)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
							.AutoWidth()
							.Padding(2.f, 0.f, 0.f, 0.f)
							.VAlign(VAlign_Center)
							.HAlign(HAlign_Fill)
						[
							SNew(SBox)
								.WidthOverride(90.f)
							[
								SNew(STextBlock).Text(LOCTEXT("Shape", "Shape"))
							]
						]
						+ SHorizontalBox::Slot()
							.Padding(2.f, 0.f, 0.f, 0.f)
							.VAlign(VAlign_Center)
							.HAlign(HAlign_Fill)
						[
							SNew(SComboBox<TSharedPtr<FString>>)
								.OptionsSource(&m_shapeList)
								.OnSelectionChanged_Raw(this, &FTelemetryVisualizerUI::OnHeatmapShapeTypeSelectionChanged)
								.OnGenerateWidget_Raw(this, &FTelemetryVisualizerUI::MakeWidgetForOption)
								.InitiallySelectedItem(m_shapeList[2])
							[
								SNew(STextBlock)
									.Text_Raw(this, &FTelemetryVisualizerUI::GetHeatmapShapeTypeItem)
							]
						]
					]
					+ SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						.Padding(40.f, 5.f, 40.f, 5.f)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
							.AutoWidth()
							.Padding(2.f, 0.f, 0.f, 0.f)
							.VAlign(VAlign_Center)
							.HAlign(HAlign_Fill)
						[
							SNew(SBox)
								.WidthOverride(90.f)
							[
								SNew(STextBlock).Text(LOCTEXT("Shape_Size", "Shape Size"))
							]
						]
						+ SHorizontalBox::Slot()
							.Padding(2.f, 0.f, 0.f, 0.f)
							.VAlign(VAlign_Center)
							.HAlign(HAlign_Fill)
						[
							SAssignNew(m_heatmapSizeText, SEditableTextBox)
								.OnTextCommitted_Raw(this, &FTelemetryVisualizerUI::HeatmapSizeTextUpdate)
								.Text(FText::FromString(FString::SanitizeFloat(MinHeatmapSize * 4)))
						]
					]
					+ SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						.Padding(40.f, 5.f, 40.f, 0.f)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
							.AutoWidth()
							.Padding(2.f, 0.f, 0.f, 0.f)
							.VAlign(VAlign_Center)
							.HAlign(HAlign_Fill)
						[
							SNew(SBox)
								.WidthOverride(90.f)
							[
								SNew(STextBlock).Text(LOCTEXT("Color_Range", "Color Range"))
							]
						]
						+ SHorizontalBox::Slot()
							.Padding(2.f, 0.f, 0.f, 0.f)
							.VAlign(VAlign_Center)
							.HAlign(HAlign_Fill)
						[
							SNew(SButton)
								.OnClicked_Raw(this, &FTelemetryVisualizerUI::DisplayDrawColor, FText::FromString("Low"), true)
							[
								SAssignNew(m_heatmapLowColorButton, SBorder)
									.Padding(FMargin(12))
									.BorderBackgroundColor(FSlateColor(m_heatmapColor.GetLowColor()))
									.BorderImage(m_heatmapColor.GetLowBrush())
							]
						]
						+ SHorizontalBox::Slot()
							.Padding(2.f, 0.f, 0.f, 0.f)
							.VAlign(VAlign_Center)
							.HAlign(HAlign_Fill)
						[
							SNew(SButton)
								.OnClicked_Raw(this, &FTelemetryVisualizerUI::DisplayDrawColor, FText::FromString("High"), true)
							[
								SAssignNew(m_heatmapHighColorButton, SBorder)
									.Padding(FMargin(12))
									.BorderBackgroundColor(FSlateColor(m_heatmapColor.GetHighColor()))
									.BorderImage(m_heatmapColor.GetHighBrush())
							]
						]
					]
					+ SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						.Padding(40.f, 0.f, 40.f, 5.f)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
							.AutoWidth()
							.Padding(94.f, 0.f, 0.f, 0.f)
							.VAlign(VAlign_Center)
							.HAlign(HAlign_Fill)
						[
							SNew(STextBlock).Text(LOCTEXT("", ""))
						]
						+ SHorizontalBox::Slot()
							.Padding(2.f, 0.f, 0.f, 0.f)
							.VAlign(VAlign_Center)
							.HAlign(HAlign_Fill)
						[
							SNew(STextBlock).Text(LOCTEXT("Minimum", "Minimum"))
						]
						+ SHorizontalBox::Slot()
							.Padding(2.f, 0.f, 0.f, 0.f)
							.VAlign(VAlign_Center)
							.HAlign(HAlign_Fill)
						[
							SNew(STextBlock).Text(LOCTEXT("Maximum", "Maximum"))
						]
					]
					+ SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						.Padding(40.f, 5.f, 40.f, 5.f)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
							.AutoWidth()
							.Padding(2.f, 0.f, 0.f, 0.f)
							.VAlign(VAlign_Center)
							.HAlign(HAlign_Fill)
						[
							SNew(SBox)
								.WidthOverride(90.f)
							[
								SNew(STextBlock).Text(LOCTEXT("Type_Range", "Type Range"))
							]
						]
						+ SHorizontalBox::Slot()
							.Padding(2.f, 0.f, 4.f, 0.f)
							.VAlign(VAlign_Center)
							.HAlign(HAlign_Fill)
						[
							SAssignNew(m_heatmapMinValueText, SEditableTextBox)
								.OnTextCommitted_Raw(this, &FTelemetryVisualizerUI::HeatmapMinValueTextUpdate)
								.Text(LOCTEXT("Min_Value", ""))
						]
						+ SHorizontalBox::Slot()
							.Padding(2.f, 0.f, 0.f, 0.f)
							.VAlign(VAlign_Center)
							.HAlign(HAlign_Fill)
						[
							SAssignNew(m_heatmapMaxValueText, SEditableTextBox)
								.OnTextCommitted_Raw(this, &FTelemetryVisualizerUI::HeatmapMaxValueTextUpdate)
								.Text(LOCTEXT("Max_Value", ""))
						]
					]
					+ SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						.Padding(40.f, 0.f, 40.f, 10.f)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
							.AutoWidth()
							.Padding(94.f, 0.f, 0.f, 0.f)
							.VAlign(VAlign_Center)
							.HAlign(HAlign_Fill)
						[
							SNew(STextBlock).Text(LOCTEXT("", ""))
						]
						+ SHorizontalBox::Slot()
							.Padding(2.f, 0.f, 0.f, 0.f)
							.VAlign(VAlign_Center)
							.HAlign(HAlign_Fill)
						[
							SNew(STextBlock).Text(LOCTEXT("Minimum", "Minimum"))
						]
						+ SHorizontalBox::Slot()
							.Padding(2.f, 0.f, 0.f, 0.f)
							.VAlign(VAlign_Center)
							.HAlign(HAlign_Fill)
						[
							SNew(STextBlock).Text(LOCTEXT("Maximum", "Maximum"))
						]
					]
					+ SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						.Padding(40.f, 10.f, 40.f, 10.f)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
							.HAlign(HAlign_Fill)
							.VAlign(VAlign_Center)
						[
							SNew(SCheckBox)
								.IsChecked_Lambda([this]() -> ECheckBoxState
								{
									if (m_heatmapOrientation) return ECheckBoxState::Checked;
									return ECheckBoxState::Unchecked;
								})
								.OnCheckStateChanged_Raw(this, &FTelemetryVisualizerUI::OnHeatmapOrientationChecked)
							[
								SNew(STextBlock)
									.Text(FText::FromString("Use Orientation"))
							]
						]
						+ SHorizontalBox::Slot()
							.HAlign(HAlign_Fill)
							.VAlign(VAlign_Center)
						[
							SNew(SCheckBox)
								.IsChecked_Lambda([this]() -> ECheckBoxState
								{
									if (m_heatmapAnimation) return ECheckBoxState::Checked;
									return ECheckBoxState::Unchecked;
								})
								.OnCheckStateChanged_Raw(this, &FTelemetryVisualizerUI::OnHeatmapAnimationChecked)
							[
								SNew(STextBlock)
									.Text(FText::FromString("Apply to Animation"))
							]
						]
					]
					+ SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						.Padding(132.f, 10.f, 40.f, 10.f)
					[
						SNew(SButton)
							.VAlign(VAlign_Center)
							.HAlign(HAlign_Center)
							.Text(LOCTEXT("Generate", "Generate"))
							.OnClicked_Raw(this, &FTelemetryVisualizerUI::GenerateHeatmap)
					]
				]
				//Heatmap Controls - End
			]
		];

	if (m_animationCurrentTime.IsValid())
	{
		m_animationCurrentTime->SetText(FString("0:00"));
	}

	if (m_animationTotalTime.IsValid())
	{
		m_animationTotalTime->SetText(FString("0:00"));
	}

	return retTab;
}


FText FTelemetryVisualizerUI::GetCurrentVizItem() const
{
	return FText::FromString(*m_vizSelection);
}

void FTelemetryVisualizerUI::OnVizSelectionChanged(TSharedPtr<FString> NewValue, ESelectInfo::Type)
{
	if (NewValue.IsValid())
	{
		ResetRange();

		m_vizSelection = NewValue;
		m_anim_Control.Stop();

		for (int i = 0; i < m_queryEventCollection.Num(); i++)
		{
			m_queryEventCollection[i].SetShouldAnimate(false);

			if (m_queryEventCollection[i].name == *NewValue)
			{
				m_anim_Control.UpdateContainer(&m_queryEventCollection[i]);
			}
		}

		if (m_animationCurrentTime.IsValid())
		{
			m_animationCurrentTime->SetText(FString("0:00"));
		}

		if (m_animationTotalTime.IsValid())
		{
			FNumberFormattingOptions format;
			format.MinimumIntegralDigits = 2;

			FTimespan totalTime = m_anim_Control.GetTimespan();
			m_animationTotalTime->SetText(FText::Format(FTextFormat::FromString("{0}:{1}"), FText::AsNumber(totalTime.GetMinutes()), FText::AsNumber(totalTime.GetSeconds(), &format)));
		}
	}
}

/////////////////////////////////ANIMATION/////////////////////////////////
void FTelemetryVisualizerUI::AnimationTimelineScrolled(float NewLevel)
{
	m_anim_Control.SetPlaybackTime(NewLevel);
}

FReply FTelemetryVisualizerUI::PlayAnimation(int speed)
{
	for (int i = 0; i < m_queryEventCollection.Num(); i++)
	{
		m_queryEventCollection[i].SetShouldDraw(false);
	}

	GenerateScrollBox();

	m_anim_Control.Play(speed);

	return FReply::Handled();
}

/////////////////////////////////HEATMAP/////////////////////////////////
void FTelemetryVisualizerUI::OnHeatmapOrientationChecked(ECheckBoxState NewState)
{
	if (NewState == ECheckBoxState::Checked)
	{
		m_heatmapOrientation = true;
	}
	else
	{
		m_heatmapOrientation = false;
	}
}

void FTelemetryVisualizerUI::OnHeatmapAnimationChecked(ECheckBoxState NewState)
{
	if (NewState == ECheckBoxState::Checked)
	{
		m_heatmapAnimation = true;
	}
	else
	{
		m_heatmapAnimation = false;
	}
}

void FTelemetryVisualizerUI::ResetRange()
{

	m_heatmapMinValue = -1;
	m_heatmapMinValueText->SetText(FText::FromString(""));
	m_heatmapMaxValue = -1;
	m_heatmapMaxValueText->SetText(FText::FromString(""));
}

void FTelemetryVisualizerUI::HeatmapSizeTextUpdate(const FText& InText, ETextCommit::Type InCommitType)
{
	FString tempText = m_heatmapSizeText->GetText().ToString();
	float value = FCString::Atof(*tempText);
	value = FMath::Max(value, MinHeatmapSize);
	value = FMath::Min(value, MaxHeatmapSize);

	ResetRange();
	m_heatmapSize = value;

	m_heatmapSizeText->SetText(FText::FromString(FString::SanitizeFloat(value)));
}

void FTelemetryVisualizerUI::HeatmapMinValueTextUpdate(const FText& InText, ETextCommit::Type InCommitType)
{
	FString tempText = m_heatmapMinValueText->GetText().ToString();
	int value = FCString::Atoi(*tempText);

	m_heatmapMinValue = FMath::Min(value, m_heatmapMaxValue);
	m_heatmapMinValueText->SetText(FText::FromString(FString::FromInt(value)));
}

void FTelemetryVisualizerUI::HeatmapMaxValueTextUpdate(const FText& InText, ETextCommit::Type InCommitType)
{
	FString tempText = m_heatmapMaxValueText->GetText().ToString();
	int value = FCString::Atoi(*tempText);

	m_heatmapMaxValue = FMath::Max(value, m_heatmapMinValue);
	m_heatmapMaxValueText->SetText(FText::FromString(FString::FromInt(value)));
}

FText FTelemetryVisualizerUI::GetHeatmapTypeItem() const
{
	FString typeString = "Population";

	switch (m_heatmapType)
	{
	case HeatmapType::Value:
		typeString = "Value";
		break;
	case HeatmapType::Population_Bar:
		typeString = "Population - Bar";
		break;
	case HeatmapType::Value_Bar:
		typeString = "Value - Bar";
		break;
	}

	return FText::FromString(*typeString);
}

FText FTelemetryVisualizerUI::GetHeatmapShapeTypeItem() const
{
	FString typeString = "Sphere";

	switch (m_heatmapShapeType)
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

void FTelemetryVisualizerUI::OnHeatmapTypeSelectionChanged(TSharedPtr<FString> NewValue, ESelectInfo::Type)
{
	if (NewValue.IsValid())
	{
		HeatmapType newType = HeatmapType::Population;

		if (*NewValue == "Value")
		{
			newType = HeatmapType::Value;
		}
		else if (*NewValue == "Population - Bar")
		{
			newType = HeatmapType::Population_Bar;
		}
		else if (*NewValue == "Value - Bar")
		{
			newType = HeatmapType::Value_Bar;
		}

		m_heatmapType = newType;
	}
}

void FTelemetryVisualizerUI::OnHeatmapShapeTypeSelectionChanged(TSharedPtr<FString> NewValue, ESelectInfo::Type)
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

		m_heatmapShapeType = newType;
	}
}

void FTelemetryVisualizerUI::SelectHeatmapDrawColor(FLinearColor NewColor)
{
	if (m_eventColor.ToString() == "High")
	{
		m_heatmapColor.SetHighColor(NewColor.Quantize());

		m_heatmapHighColorButton->SetBorderBackgroundColor(FSlateColor(m_heatmapColor.GetHighColor()));
		m_heatmapHighColorButton->SetBorderImage(m_heatmapColor.GetHighBrush());
	}
	else
	{
		m_heatmapColor.SetLowColor(NewColor.Quantize());

		m_heatmapLowColorButton->SetBorderBackgroundColor(FSlateColor(m_heatmapColor.GetLowColor()));
		m_heatmapLowColorButton->SetBorderImage(m_heatmapColor.GetLowBrush());
	}
}

//Generates a heatmap for all events
FReply FTelemetryVisualizerUI::GenerateHeatmap()
{
	SEventEditorContainer* collection = nullptr;

	for (int i = 0; i < m_queryEventCollection.Num(); i++)
	{
		m_queryEventCollection[i].SetShouldAnimate(false);
		m_queryEventCollection[i].SetShouldDraw(false);

		if (m_queryEventCollection[i].name == *m_vizSelection)
		{
			collection = &m_queryEventCollection[i];
		}
	}

	if (collection == nullptr)
	{
		return FReply::Handled();
	}

	return GenerateHeatmap(collection, 0, collection->events.Num() - 1);
}

//Generate a heatmap for the range of events
FReply FTelemetryVisualizerUI::GenerateHeatmap(SEventEditorContainer* collection, int first, int last)
{
	DestroyActors();
	UWorld* currentTarget = GetLocalWorld();

	if (currentTarget != nullptr && collection != nullptr)
	{
		if ((first == 0 && first == last) || (first == collection->events.Num() - 1 && first == last))
		{
			first = 0;
			last = collection->events.Num() - 1;
		}

		//Segment the world in to blocks of the specified size encompassing all points
		FBox range = collection->GetPointRange(first, last);
		float extent = m_heatmapSize / 2;
		FVector extents(extent, extent, extent);
		TArray<FBox> parts;

		range.Max.X += m_heatmapSize;
		range.Max.Y += m_heatmapSize;
		range.Max.Z += m_heatmapSize;

		for (int x = range.Min.X + extent; x < range.Max.X; x += m_heatmapSize)
		{
			for (int y = range.Min.Y + extent; y < range.Max.Y; y += m_heatmapSize)
			{
				for (int z = range.Min.Z + extent; z < range.Max.Z; z += m_heatmapSize)
				{
					parts.Add(FBox::BuildAABB(FVector(x, y, z), extents));
				}
			}
		}

		if (parts.Num() > 0)
		{
			//For each segment, collect all of the points inside and decide what data to watch based on the heatmap type
			TArray<int> numValues;
			TArray<int> values;
			TArray<FVector> orientation;
			float scaledHeatmapSize = m_heatmapSize / 100;
			ATelemetryEvent* tempActor;
			FString tempName = "Heatmap";

			FActorSpawnParameters params;
			params.Name = *tempName;

			numValues.SetNumZeroed(parts.Num());
			values.SetNumZeroed(parts.Num());
			orientation.SetNumZeroed(parts.Num());

			if (m_heatmapMinValue == -1 && m_heatmapMaxValue == -1)
			{
				int largestValue = 0;
				int largestNumValue = 0;

				if (m_heatmapOrientation)
				{
					for (int i = 0; i < parts.Num(); i++)
					{
						for (int j = first; j <= last; j++)
						{
							if (parts[i].IsInside(collection->events[j]->point))
							{
								numValues[i]++;
								orientation[i] += collection->events[j]->orientation;
								values[i] += collection->events[j]->value;
							}
						}

						largestValue = FMath::Max(largestValue, values[i]);
						largestNumValue = FMath::Max(largestNumValue, numValues[i]);
						orientation[i] = orientation[i] / numValues[i];
					}
				}
				else
				{
					for (int i = 0; i < parts.Num(); i++)
					{
						for (int j = first; j <= last; j++)
						{
							if (parts[i].IsInside(collection->events[j]->point))
							{
								numValues[i]++;
								values[i] += collection->events[j]->value;
							}
						}

						largestValue = FMath::Max(largestValue, values[i]);
						largestNumValue = FMath::Max(largestNumValue, numValues[i]);
					}
				}

				m_heatmapMinValue = 0;

				if (m_heatmapType == HeatmapType::Value || m_heatmapType == HeatmapType::Value_Bar)
				{
					if (collection->IsPercentage())
					{
						m_heatmapMaxValue = 100;
					}
					else
					{
						m_heatmapMaxValue = largestValue;
					}
				}
				else
				{
					m_heatmapMaxValue = largestNumValue;
				}

				m_heatmapMinValueText->SetText(FText::FromString("0"));
				m_heatmapMaxValueText->SetText(FText::FromString(FString::FromInt(m_heatmapMaxValue)));
			}
			else
			{
				if (m_heatmapOrientation)
				{
					for (int i = 0; i < parts.Num(); i++)
					{
						for (int j = first; j <= last; j++)
						{
							if (parts[i].IsInside(collection->events[j]->point))
							{
								numValues[i]++;
								orientation[i] += collection->events[j]->orientation;
								values[i] += collection->events[j]->value;
							}
						}

						orientation[i] = orientation[i] / numValues[i];
					}
				}
				else
				{
					for (int i = 0; i < parts.Num(); i++)
					{
						for (int j = first; j <= last; j++)
						{
							if (parts[i].IsInside(collection->events[j]->point))
							{
								numValues[i]++;
								values[i] += collection->events[j]->value;
							}
						}
					}
				}
			}

			//For each section, add instanced mesh to the master mesh and set its shape and color as needed
			tempActor = currentTarget->SpawnActor<ATelemetryEvent>(FVector::ZeroVector, FRotator::ZeroRotator, params);
			tempActor->SetActorLabel(*tempName);

			float tempValue;
			float tempColorValue;

			if (m_heatmapType == HeatmapType::Value)
			{
				for (int i = 0; i < parts.Num(); i++)
				{
					if (values[i] > 0)
					{
						tempValue = ((float)values[i] / numValues[i]) / m_heatmapMaxValue;
						tempColorValue = (((float)values[i] / numValues[i]) - m_heatmapMinValue) / (m_heatmapMaxValue - m_heatmapMinValue);
						tempColorValue = FMath::Max(tempColorValue, 0.f);
						tempColorValue = FMath::Min(tempColorValue, (float)m_heatmapMaxValue);

						tempActor->AddEvent(parts[i].GetCenter(), orientation[i], m_heatmapColor.GetColorFromRange(tempColorValue), m_heatmapShapeType, scaledHeatmapSize, tempValue);
					}
				}
			}
			else if (m_heatmapType == HeatmapType::Population)
			{
				for (int i = 0; i < parts.Num(); i++)
				{
					if (numValues[i] > 0)
					{
						tempValue = (float)numValues[i] / m_heatmapMaxValue;
						tempColorValue = (float)(numValues[i] - m_heatmapMinValue) / (m_heatmapMaxValue - m_heatmapMinValue);
						tempColorValue = FMath::Max(tempColorValue, 0.f);
						tempColorValue = FMath::Min(tempColorValue, (float)m_heatmapMaxValue);

						tempActor->AddEvent(parts[i].GetCenter(), orientation[i], m_heatmapColor.GetColorFromRange(tempColorValue), m_heatmapShapeType, scaledHeatmapSize, tempValue);
					}
				}
			}
			else if (m_heatmapType == HeatmapType::Value_Bar)
			{
				float tempHeight;

				for (int i = 0; i < parts.Num(); i++)
				{
					if (values[i] > 0)
					{
						tempValue = ((float)values[i] / numValues[i]) / m_heatmapMaxValue;
						tempColorValue = (((float)values[i] / numValues[i]) - m_heatmapMinValue) / (m_heatmapMaxValue - m_heatmapMinValue);
						tempColorValue = FMath::Max(tempColorValue, 0.f);
						tempColorValue = FMath::Min(tempColorValue, (float)m_heatmapMaxValue);
						tempHeight = (((float)values[i] / numValues[i]) / m_heatmapMaxValue) * scaledHeatmapSize;

						tempActor->AddEvent(parts[i].GetCenter(), FVector::ZeroVector, m_heatmapColor.GetColorFromRange(tempColorValue), EventType::Cube,
							FBox::BuildAABB(FVector(0, 0, (tempHeight / 2) * 100), FVector(scaledHeatmapSize, scaledHeatmapSize, tempHeight)), tempValue);
					}
				}
			}
			else if (m_heatmapType == HeatmapType::Population_Bar)
			{
				float tempHeight;

				for (int i = 0; i < parts.Num(); i++)
				{
					if (numValues[i] > 0)
					{
						tempValue = (float)numValues[i] / m_heatmapMaxValue;
						tempColorValue = (float)(numValues[i] - m_heatmapMinValue) / (m_heatmapMaxValue - m_heatmapMinValue);
						tempColorValue = FMath::Max(tempColorValue, 0.f);
						tempColorValue = FMath::Min(tempColorValue, (float)m_heatmapMaxValue);
						tempHeight = ((float)numValues[i] / m_heatmapMaxValue) * scaledHeatmapSize;

						tempActor->AddEvent(parts[i].GetCenter(), FVector::ZeroVector, m_heatmapColor.GetColorFromRange(tempColorValue), EventType::Cube,
							FBox::BuildAABB(FVector(0, 0, (tempHeight / 2) * 100), FVector(scaledHeatmapSize, scaledHeatmapSize, tempHeight)), tempValue);
					}
				}
			}

			tempName = "/TelemetryEvents";
			tempActor->SetFolderPath(*tempName);

			m_eventActors.Add(tempActor);
		}
	}

	return FReply::Handled();
}

//void FTelemetryVisualizerUI::CreateHeatmapActor(UWorld* drawTarget, int index, FVector location, int value, int range, EventType shape, float size)
//{
//	ATelemetryEvent* tempActor;
//	FString tempName;
//		
//	tempName = "Heatmap " + FString::FromInt(index);
//
//	FActorSpawnParameters params;
//	params.Name = *tempName;
//
//	tempActor = drawTarget->SpawnActor<ATelemetryEvent>(FVector::ZeroVector, FRotator::ZeroRotator, params);
//	tempActor->SetActorLabel(*tempName);
//	tempActor->AddEvent(location, m_heatmapColor.GetColorFromRange((float)value / range), shape, size / 100);
//
//	tempActor->SetValue(value);
//	tempActor->SetRange(range);
//
//	tempName = "/TelemetryEvents/Heatmap";
//	tempActor->SetFolderPath(*tempName);
//
//	m_eventActors.Add(tempActor);
//}
//
//void FTelemetryVisualizerUI::CombineHeatmap()
//{
//	TArray<AStaticMeshActor*> AllActors;
//	TArray<UPrimitiveComponent*> AllComponents;
//	FString BasePackageName;
//	FMeshMergingSettings MeshMergingSettings;
//
//	if (m_eventActors.Num() == 0) return;
//
//	AllActors.SetNum(m_eventActors.Num());
//	AllComponents.SetNum(m_eventActors.Num());
//
//	for (int i = 0; i < m_eventActors.Num(); i++)
//	{
//		AllActors[i] = (AStaticMeshActor*)m_eventActors[i];
//
//		TInlineComponentArray<UStaticMeshComponent*> ComponentArray;
//		AllActors[i]->GetComponents<UStaticMeshComponent>(ComponentArray);
//
//		for (UStaticMeshComponent* MeshCmp : ComponentArray)
//		{
//			if (MeshCmp->GetStaticMesh() && MeshCmp->GetStaticMesh()->RenderData.IsValid())
//			{
//				AllComponents[i] = MeshCmp;
//				break;
//			}
//		}
//	}
//
//	TArray<UObject*> CreatedAssets;
//	FVector MergedActorLocation;
//	const float ScreenAreaSize = TNumericLimits<float>::Max();
//	const IMeshMergeUtilities& MeshUtilities = FModuleManager::Get().LoadModuleChecked<IMeshMergeModule>("MeshMergeUtilities").GetUtilities();
//	MeshUtilities.MergeComponentsToStaticMesh(
//		AllComponents,
//		AllActors[0]->GetWorld(),
//		MeshMergingSettings,
//		nullptr,
//		nullptr,
//		BasePackageName,
//		CreatedAssets,
//		MergedActorLocation,
//		ScreenAreaSize,
//		true);
//
//	UWorld* currentTarget = GetLocalWorld();
//
//	if (currentTarget != nullptr)
//	{
//		DestroyActors();
//		AStaticMeshActor* tempActor;
//		FString tempName;
//		FActorSpawnParameters params;
//		UStaticMesh* mesh;
//		UMaterialInterface * Material;
//		UMaterialInstanceDynamic* matInstance;
//
//		if (m_material == nullptr)
//		{
//			for (TObjectIterator<UMaterial> It; It; ++It)
//			{
//				if ((*It)->GetName() == "M_Telemetry")
//				{
//					m_material = *It;
//					break;
//				}
//			}
//		}
//
//		for (int i = 0; i < CreatedAssets.Num(); i++)
//		{
//			if (CreatedAssets[i]->IsA(UStaticMesh::StaticClass()))
//			{
//				mesh = (UStaticMesh*)CreatedAssets[i];
//				tempName = "Heatmap " + FString::FromInt(i);
//				params.Name = *tempName;
//
//				tempActor = currentTarget->SpawnActor<AStaticMeshActor>(MergedActorLocation, FRotator::ZeroRotator, params);
//				tempActor->SetActorLabel(*tempName);
//				tempActor->GetStaticMeshComponent()->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
//				tempActor->GetStaticMeshComponent()->SetStaticMesh(mesh);
//
//				for (int j = 0; j < tempActor->GetStaticMeshComponent()->GetNumMaterials(); j++)
//				{
//					if (m_material != nullptr)
//					{
//						tempActor->GetStaticMeshComponent()->SetMaterial(j, m_material);
//
//						Material = tempActor->GetStaticMeshComponent()->GetMaterial(j);
//						matInstance = tempActor->GetStaticMeshComponent()->CreateDynamicMaterialInstance(j, Material);
//
//						if (matInstance != nullptr)
//						{
//							matInstance->SetVectorParameterValue("Color", FLinearColor(FColor::Green));
//							tempActor->GetStaticMeshComponent()->SetMaterial(j, matInstance);
//						}
//					}
//				}
//
//				tempName = "/TelemetryEvents/Heatmap";
//				tempActor->SetFolderPath(*tempName);
//
//				m_eventActors.Add(tempActor);
//			}
//		}
//	}
//}
