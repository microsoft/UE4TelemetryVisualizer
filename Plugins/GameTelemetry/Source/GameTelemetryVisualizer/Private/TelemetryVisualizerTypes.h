//--------------------------------------------------------------------------------------
// TelemetryVisualizerTypes.h
//
// Provides interactivity for Telemetry events
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#pragma once
#include "Brushes/SlateColorBrush.h"
#include "Query/TelemetryQuery.h"

//Setting whether to draw recieved events by default
static const bool DefaultDrawSetting = true;

//Set of default colors for drawing events
static const TArray<FColor> DefaultColors =
{
	FColor::Red,
	FColor::Green,
	FColor::Blue,
	FColor::Yellow,
	FColor::Cyan,
	FColor::Magenta,
	FColor::Orange,
	FColor::Purple,
	FColor::Turquoise,
	FColor::Silver,
	FColor::Emerald
};

//Primary container for events managed by the UI
struct STelemetryEvent
{
	TCHAR user[128];
	TCHAR build[128];
	TCHAR name[128];
	TCHAR category[128];
	TCHAR session[38];
	FVector point;
	FVector orientation;
	FDateTime time;
	float value;
	bool isPct;

	STelemetryEvent() : name(TEXT("\0")), category(TEXT("\0")), session(TEXT("\0")), user(TEXT("\0")), build(TEXT("\0")), point(FVector::ZeroVector), orientation(FVector::ZeroVector), time(0), value(0.f),isPct(false) {};
	STelemetryEvent(FString inName, FString inCategory, FString inSession, FString inBuild, FString inUser, FVector point, FDateTime time, float value, bool isPct)
		: point(point), time(time), value(value), isPct(isPct)
	{
		orientation = FVector::ZeroVector;
		SetName(inName);
		SetCategory(inCategory);
		SetSession(inSession);
		SetBuild(inBuild);
		SetUser(inUser);
	};

	STelemetryEvent(FString inName, FString inCategory, FString inSession, FString inBuild, FString inUser, FVector point, FVector orientation, FDateTime time, float value, bool isPct)
		: point(point), orientation(orientation), time(time), value(value), isPct(isPct)
	{
		SetName(inName);
		SetCategory(inCategory);
		SetSession(inSession);
		SetBuild(inBuild);
		SetUser(inUser);
	};

	void SetName(FString inName)
	{
		if (inName.Len() < 128)
		{
			FCString::Strcpy(name, *inName);
		}
	}

	FString GetName()
	{
		return FString(name);
	}

	void SetCategory(FString inCategory)
	{
		if (inCategory.Len() < 128)
		{
			FCString::Strcpy(category, *inCategory);
		}
	}

	FString GetCategory()
	{
		return FString(category);
	}

	void SetBuild(FString inBuild)
	{
		if (inBuild.Len() < 128)
		{
			FCString::Strcpy(build, *inBuild);
		}
	}

	FString GetBuild()
	{
		return FString(build);
	}

	void SetUser(FString inUser)
	{
		if (inUser.Len() < 128)
		{
			FCString::Strcpy(user, *inUser);
		}
	}

	FString GetUser()
	{
		return FString(user);
	}

	void SetSession(FString inSession)
	{
		if (inSession.Len() < 38)
		{
			FCString::Strcpy(session, *inSession);
		}
	}

	FString GetSession()
	{
		return FString(session);
	}

	bool IsPercentage()
	{
		return isPct;
	}
};

//Wrapper for the event container with draw information
struct SEventEditorContainer
{
private:
	FColor color;
	FSlateBrush colorBrush;
	bool shouldDraw;
	bool shouldAnimate;
	bool isPct;
	FDateTime timeStart;
	FDateTime timeEnd;
	EventType type;

public:
	FString name;
	FString session;
	TArray<TSharedPtr<STelemetryEvent>> events;

	SEventEditorContainer() : shouldDraw(DefaultDrawSetting), shouldAnimate(false), isPct(false), color(FColor::Red), colorBrush((FSlateBrush)FSlateColorBrush(FColor::Red)), type(EventType::Sphere)
	{
	}

	SEventEditorContainer(FString name, int index) : name(name), shouldDraw(DefaultDrawSetting), shouldAnimate(false), isPct(false), color(FColor::Red), colorBrush((FSlateBrush)FSlateColorBrush(FColor::Red)), type(EventType::Sphere)
	{
		color = DefaultColors[index % DefaultColors.Num()];
		colorBrush = (FSlateBrush)FSlateColorBrush(color);
	}

	SEventEditorContainer(TArray<FSimpleEvent> iEvents) : shouldDraw(DefaultDrawSetting), shouldAnimate(false), color(FColor::Red), colorBrush((FSlateBrush)FSlateColorBrush(FColor::Red)), type(EventType::Sphere)
	{
		if (iEvents.Num() > 0)
		{
			Fill(iEvents);
			name = iEvents[0].GetName();
			isPct = events[0]->IsPercentage();
			SetupTimes();
		}
	}

	void AddEvent(FSimpleEvent newEvent)
	{
		FString valueName;
		double value = 0.f;
		bool isPct = false;

		//Check for a specified value the event wants to draw
		if (newEvent.GetString(L"disp_val", valueName))
		{
			value = newEvent.GetNumber(valueName);
			isPct = valueName.StartsWith("pct_");
		}

		events.Emplace(MakeShareable(new STelemetryEvent(
			newEvent.GetName(),
			newEvent.GetCategory(),
			newEvent.GetSessionId(),
			newEvent.GetBuildType() + L" " + newEvent.GetBuildId() + L" " + newEvent.GetPlatform(),
			newEvent.GetUserId(),
			newEvent.GetPlayerPosition(),
			newEvent.GetPlayerDirection(),
			newEvent.GetTime(),
			(float)value,
			isPct)));
		SetupTimes();
	}

	//Add an array of query results
	void Fill(TArray<FSimpleEvent> iEvents)
	{
		for (auto newEvent : iEvents)
		{
			AddEvent(newEvent);
		}
	}

	//Keep a timespan for the event collection
	void SetupTimes()
	{
		timeStart = events[events.Num() - 1]->time;
		timeEnd = events[0]->time;
	}

	//Provides the total timespan for all events
	FTimespan GetTimespan()
	{
		return timeEnd - timeStart;
	}

	//Provides a percent location based on tick time
	float GetTimeScaleFromTime(FTimespan inTime)
	{
		return (float)FTimespan::Ratio(inTime, timeEnd - timeStart);
	}

	//Provides a percent location based on the element
	float GetEventTimeScale(int i)
	{
		return (float)FTimespan::Ratio(events[events.Num() - 1 - i]->time - timeStart, timeEnd - timeStart);
	}

	//Provides an element based on the percent location
	int GetEventIndexForTimeScale(float scale)
	{
		int i;
		FDateTime targetValue = timeStart + ((timeEnd - timeStart) * scale);

		for (i = events.Num() - 1; i > 0; i--)
		{
			if (events[i]->time > targetValue) break;
		}

		return i;
	}

	FSlateBrush* GetBrush()
	{
		return &colorBrush;
	}

	void SetColor(FColor inColor)
	{
		color = inColor;
		colorBrush = (FSlateBrush)FSlateColorBrush(color);
	}

	FColor GetColor()
	{
		return color;
	}

	bool ShouldDraw()
	{
		return shouldDraw;
	}

	void SetShouldDraw(bool inBool)
	{
		shouldDraw = inBool;
	}

	bool ShouldAnimate()
	{
		return shouldAnimate;
	}

	void SetShouldAnimate(bool inBool)
	{
		shouldAnimate = inBool;
	}

	void SetShapeType(EventType inType)
	{
		type = inType;
	}

	EventType GetShapeType()
	{
		return type;
	}

	//Gets the box for where events happen (for heatmap)
	FBox GetPointRange()
	{
		return GetPointRange(0, events.Num() - 1);
	}

	//Gets the box for where specified events happen (for heatmap animations)
	FBox GetPointRange(int start, int end)
	{
		FBox range(FVector(FLT_MAX, FLT_MAX, FLT_MAX), FVector(-FLT_MAX, -FLT_MAX, -FLT_MAX));

		for (int i = start; i <= end; i++)
		{
			range.Min.X = FMath::Min(range.Min.X, events[i]->point.X);
			range.Min.Y = FMath::Min(range.Min.Y, events[i]->point.Y);
			range.Min.Z = FMath::Min(range.Min.Z, events[i]->point.Z);
			range.Max.X = FMath::Max(range.Max.X, events[i]->point.X);
			range.Max.Y = FMath::Max(range.Max.Y, events[i]->point.Y);
			range.Max.Z = FMath::Max(range.Max.Z, events[i]->point.Z);
		}

		return range;
	}

	bool IsPercentage()
	{
		return isPct;
	}
};

//Types of heatmaps offered
static enum HeatmapType
{
	Population,
	Value,
	Population_Bar,
	Value_Bar
};

static enum AnimationState
{
	Stopped,
	Playing,
	Paused,
};

struct ColorRange
{
	uint8 A;
	uint8 B;
	uint8 G;
	uint8 R;
};

//Wraps colors and brushes for heatmap settings
struct HeatmapColors
{
private:
	FColor lowColor;
	FSlateBrush lowColorBrush;
	FColor highColor;
	FSlateBrush highColorBrush;
	ColorRange range;

	void UpdateRange()
	{
		range.A = highColor.A - lowColor.A;
		range.B = highColor.B - lowColor.B;
		range.G = highColor.G - lowColor.G;
		range.R = highColor.R - lowColor.R;
	}

public:
	HeatmapColors() : lowColor(FColor::Green), highColor(FColor::Red), lowColorBrush((FSlateBrush)FSlateColorBrush(FColor::Green)), highColorBrush((FSlateBrush)FSlateColorBrush(FColor::Red))
	{
		lowColor.A = 128;
		highColor.A = 128;
		UpdateRange();
	}

	FSlateBrush* GetLowBrush()
	{
		return &lowColorBrush;
	}

	FSlateBrush* GetHighBrush()
	{
		return &highColorBrush;
	}

	void SetLowColor(FColor inColor)
	{
		lowColor = inColor;
		lowColorBrush = (FSlateBrush)FSlateColorBrush(lowColor);
		UpdateRange();
	}

	void SetHighColor(FColor inColor)
	{
		highColor = inColor;
		highColorBrush = (FSlateBrush)FSlateColorBrush(highColor);
		UpdateRange();
	}

	FColor GetLowColor()
	{
		return lowColor;
	}

	FColor GetHighColor()
	{
		return highColor;
	}

	FColor GetColorFromRange(float location)
	{
		FColor retColor = lowColor;

		retColor.A += (uint8)(range.A * location);
		retColor.R += (uint8)(range.R * location);
		retColor.G += (uint8)(range.G * location);
		retColor.B += (uint8)(range.B * location);

		return retColor;
	}
};

//Strings associated with different viz settings for UI
static const TArray<FString> ShapeListStrings =
{
	"Sphere",
	"Cone",
	"Cube",
	"Cylinder",
	"Plane"
};

static const TArray<FString> HeatmapTypeStrings =
{
	"Population",
	"Value",
	"Population - Bar",
	"Value - Bar"
};

static const TArray<FString> AndOrStrings =
{
	"Or",
	"And"
};

static const TArray<FString> QueryFieldStrings =
{
	"Category",
	"Build ID",
	"Build Type",
	"Client ID",
	"Platform",
	"Process ID",
	"Session ID",
	"User ID"
};

static const TArray<FString> QueryExpectedStrings =
{
	"cat",
	"build_id",
	"build_type",
	"client_id",
	"platform",
	"process_id",
	"session_id",
	"user_id"
};

static enum QueryField
{
	Category,
	Build_ID,
	Build_Type,
	Client_ID,
	Platform,
	Process_ID,
	Session_ID,
	User_ID
};

static const TArray<FString> QueryOperatorStrings =
{
	"==",
	"<>",
	">",
	"<",
	">=",
	"<="
};

static enum QueryOperator
{
	Equal,
	Not_Equal,
	GreaterThan,
	LessThan,
	GreaterThanOrEqual,
	LessThanOrEqual
};

//Container for each UI line of the query
struct SQuerySetting
{
	bool isAnd;
	QueryField Field;
	QueryOperator Operator;
	FString Value;

	SQuerySetting() : isAnd(false), Field(QueryField::Category), Operator(QueryOperator::Equal), Value("")
	{
	}
};

//Manages animation state based on the time an animation was started
struct AnimationController
{
private:
	AnimationState state;

	int playSpeed;
	int nextIndexDraw;
	bool needRefresh;

	FDateTime localStartTime;

	SEventEditorContainer* eventContainer;

public:
	AnimationController()
	{
		state = AnimationState::Stopped;
		playSpeed = 0;
		nextIndexDraw = 0;
		needRefresh = true;
		eventContainer = nullptr;
	}

	AnimationController(SEventEditorContainer* newContainer)
	{
		state = AnimationState::Stopped;
		playSpeed = 0;
		nextIndexDraw = 0;
		needRefresh = true;
		eventContainer = newContainer;
	}

	bool IsPlaying() { return state == AnimationState::Playing; }
	bool IsPaused() { return state == AnimationState::Paused; }
	bool IsStopped() { return state == AnimationState::Stopped; }

	SEventEditorContainer* GetEventContainer() { return eventContainer;	}

	FTimespan GetTimespan()
	{
		if (eventContainer != nullptr)
		{
			return eventContainer->GetTimespan();
		}

		return FTimespan(0);
	}
			

	void UpdateContainer(SEventEditorContainer* newContainer) { eventContainer = newContainer; }

	FColor GetColor()
	{
		if (eventContainer != nullptr)
		{
			return eventContainer->GetColor();
		}

		return FColor::Red;
	}

	EventType GetShapeType()
	{
		if (eventContainer != nullptr)
		{
			return eventContainer->GetShapeType();
		}

		return EventType::Sphere;
	}

	bool NeedRefresh()
	{
		bool ret = needRefresh;
		needRefresh = false;
		return ret;
	}

	void Play(int speed)
	{
		if (eventContainer == nullptr) return;

		if (state == AnimationState::Playing && playSpeed == speed)
		{
			//Already playing at this speed, so pause
			Pause();
		}
		else if (state == AnimationState::Playing)
		{
			//Need to play at different speed, so reset the start time to when it would have started
			//at the given speed
			localStartTime = FDateTime::UtcNow();

			if (nextIndexDraw > 0)
			{
				localStartTime -= eventContainer->events[nextIndexDraw]->time - eventContainer->events[eventContainer->events.Num() - 1]->time;
			}
		}
		else if (state == AnimationState::Stopped)
		{
			//Start playing from the beginning
			needRefresh = true;

			if(speed >= 0)
			{
				nextIndexDraw = eventContainer->events.Num() - 1;
			}
			else
			{
				nextIndexDraw = 0;
			}

			localStartTime = FDateTime::UtcNow();
			state = AnimationState::Playing;
			eventContainer->SetShouldAnimate(true);
		}
		else if (state == AnimationState::Paused)
		{
			//Resume playing, so update start time to include the time this was paused
			localStartTime = FDateTime::UtcNow();

			if (nextIndexDraw > 0)
			{
				localStartTime -= eventContainer->events[nextIndexDraw]->time - eventContainer->events[eventContainer->events.Num() - 1]->time;
			}

			state = AnimationState::Playing;
			eventContainer->SetShouldAnimate(true);
		}

		playSpeed = speed;
	}

	void Pause()
	{
		if (eventContainer == nullptr) return;
		state = AnimationState::Paused;
		eventContainer->SetShouldAnimate(false);
	}

	void Stop()
	{
		if (eventContainer == nullptr) return;
		state = AnimationState::Stopped;
		playSpeed = 0;
		nextIndexDraw = eventContainer->events.Num() - 1;
		eventContainer->SetShouldAnimate(false);
	}

	int GetPlaySpeed() { return playSpeed; }
	void SetPlaySpeed(int speed) { playSpeed = speed; }
	bool ShouldAnimate() { return eventContainer->ShouldAnimate(); }

	int GetNextIndex() { return nextIndexDraw; }
	int GetPrevIndex()
	{
		if (nextIndexDraw <= 0)
		{
			return eventContainer->events.Num() - 1;
		}

		return nextIndexDraw - 1;
	}

	//Provides an array of pointers to events that are ready to draw for the animation
	TArray<TSharedPtr<STelemetryEvent>> GetNextEvents()
	{
		if (nextIndexDraw <= 0)
		{
			Stop();
			eventContainer->SetShouldDraw(true);
		}

		TArray<TSharedPtr<STelemetryEvent>> newArray;

		if (playSpeed >= 0)
		{
			FDateTime tempTime = eventContainer->events[eventContainer->events.Num() - 1]->time + ((FDateTime::UtcNow() - localStartTime) * playSpeed);

			while (nextIndexDraw >= 0 && eventContainer->events[nextIndexDraw]->time < tempTime)
			{
				newArray.Add(eventContainer->events[nextIndexDraw]);
				nextIndexDraw--;
			}

			if (newArray.Num() == 0 && nextIndexDraw >= 0)
			{
				//Skip ahead if the gap between events is too long
				FTimespan timeToNext = eventContainer->events[nextIndexDraw]->time - tempTime;
				if (timeToNext > FTimespan(0, 0, 30))
				{
					localStartTime -= timeToNext - FTimespan(0, 0, 5);
				}
			}
		}

		return newArray;
	}

	//Provides the index of the next event to draw
	int GetNextEventCount()
	{
		if (nextIndexDraw <= 0)
		{
			Stop();
			//eventContainer->SetShouldDraw(true);
		}

		bool newEvents = false;

		if (playSpeed >= 0)
		{
			FDateTime tempTime = eventContainer->events[eventContainer->events.Num() - 1]->time + ((FDateTime::UtcNow() - localStartTime) * playSpeed);

			while (nextIndexDraw >= 0 && eventContainer->events[nextIndexDraw]->time < tempTime)
			{
				newEvents = true;
				nextIndexDraw--;
			}

			if (!newEvents && nextIndexDraw >= 0)
			{
				//Skip ahead if the gap between events is too long
				FTimespan timeToNext = eventContainer->events[nextIndexDraw]->time - tempTime;
				if (timeToNext > FTimespan(0, 0, 30))
				{
					localStartTime -= timeToNext - FTimespan(0, 0, 5);
				}
			}

			if (nextIndexDraw < 0)
			{
				nextIndexDraw = 0;
			}
		}

		return nextIndexDraw;
	}

	//Provides an array of pointers to events that are ready to draw for the animation (in reverse)
	TArray<TSharedPtr<STelemetryEvent>> GetPrevEvents()
	{
		TArray<TSharedPtr<STelemetryEvent>> newArray;

		if (playSpeed < 0)
		{
			if (nextIndexDraw >= eventContainer->events.Num() - 1)
			{
				Stop();
				eventContainer->SetShouldDraw(false);
			}

			FDateTime tempTime = eventContainer->events[0]->time + ((FDateTime::UtcNow() - localStartTime) * playSpeed);

			while (nextIndexDraw < eventContainer->events.Num() && eventContainer->events[nextIndexDraw]->time > tempTime)
			{
				newArray.Add(eventContainer->events[nextIndexDraw]);
				nextIndexDraw++;
			}

			if (newArray.Num() == 0 && nextIndexDraw < eventContainer->events.Num())
			{
				//Skip ahead if the gap between events is too long
				FTimespan timeToNext = eventContainer->events[nextIndexDraw]->time - tempTime;
				if (timeToNext > FTimespan(0, 0, 30))
				{
					localStartTime += timeToNext - FTimespan(0, 0, 5);
				}
			}
		}

		return newArray;
	}

	//Provides the index of the next event to draw (in reverse)
	int GetPrevEventCount()
	{
		if (playSpeed < 0)
		{
			if (nextIndexDraw >= eventContainer->events.Num() - 1)
			{
				Stop();
				//eventContainer->SetShouldDraw(false);
			}

			FDateTime tempTime = eventContainer->events[0]->time + ((FDateTime::UtcNow() - localStartTime) * playSpeed);
			bool newEvents = false;

			while (nextIndexDraw < eventContainer->events.Num() && eventContainer->events[nextIndexDraw]->time > tempTime)
			{
				newEvents = true;
				nextIndexDraw++;
			}

			if (!newEvents && nextIndexDraw < eventContainer->events.Num())
			{
				//Skip ahead if the gap between events is too long
				FTimespan timeToNext = eventContainer->events[nextIndexDraw]->time - tempTime;
				if (timeToNext > FTimespan(0, 0, 30))
				{
					localStartTime += timeToNext - FTimespan(0, 0, 5);
				}
			}

			if (nextIndexDraw >= eventContainer->events.Num())
			{
				nextIndexDraw = eventContainer->events.Num() - 1;
			}
		}

		return nextIndexDraw;
	}

	//Set a time for playback  (0 to 1)
	void SetPlaybackTime(float scale)
	{
		if (eventContainer == nullptr) return;

		int newIndexDraw = GetEventIndexForTimeScale(scale);

		localStartTime = FDateTime::UtcNow();

		if (newIndexDraw > 0 && newIndexDraw < eventContainer->events.Num() - 1)
		{
			localStartTime -= eventContainer->events[newIndexDraw]->time - eventContainer->events[eventContainer->events.Num() - 1]->time;
		}

		needRefresh = true;
	}

	FTimespan GetCurrentPlayTime()
	{
		if (playSpeed >= 0)
		{
			return (FDateTime::UtcNow() - localStartTime) * playSpeed;
		}
		else
		{
			return eventContainer->GetTimespan() + (FDateTime::UtcNow() - localStartTime) * playSpeed;
		}
	}

	float GetTimeScaleFromTime()
	{
		return eventContainer->GetTimeScaleFromTime((FDateTime::UtcNow() - localStartTime) * playSpeed);
	}

	float GetEventTimeScale(int i)
	{
		return eventContainer->GetEventTimeScale(i);
	}

	int GetEventIndexForTimeScale(float scale)
	{
		return eventContainer->GetEventIndexForTimeScale(scale);
	}
};
