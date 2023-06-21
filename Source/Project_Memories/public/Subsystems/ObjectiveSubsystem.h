// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ObjectiveActors/ObjectiveClock.h"
#include "Subsystems/WorldSubsystem.h"
#include "ObjectiveSubsystem.generated.h"

/**
 * 
 */

class AObjectiveClock;

UENUM()
enum class EObjectiveType
{
	OT_SpotLight,

	OT_FireBowl,

	OT_Clock,

	
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnObjectiveCompletedDelegate, TWeakObjectPtr<AActor>, ObjectiveActor, EObjectiveType, Type);
UCLASS()
class PROJECT_MEMORIES_API UObjectiveSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	
	void TrackClock(TWeakObjectPtr<AObjectiveClock> InClock, bool bIsLeading);

	void SetClockSyncRequestDelegate(FSyncRequestDelegate& SyncDelegate);

	void OnSyncRequest(FRotator TargetRotation, float Tolerance);

private:
	//Clock Begin
	TArray<TWeakObjectPtr<AObjectiveClock>> ListeningClocks;
	TWeakObjectPtr<AObjectiveClock> LeadingClock;

	TMap<EObjectiveType, bool> ObjectiveTracker;

	//ClockEnd;
};
