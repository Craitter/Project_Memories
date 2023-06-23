// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MemoriesCharacter.h"
#include "ObjectiveActors/ObjectiveClock.h"
#include "Subsystems/WorldSubsystem.h"
#include "ObjectiveSubsystem.generated.h"

/**
 * 
 */

class ATorchBasin;
class AMovableActor;
class ALightSourceAndTargtetActor;
class AObjectiveClock;
class AMemoriesCharacter;

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
	UFUNCTION(BlueprintCallable)
	bool IsGameFInished();
	
	UObjectiveSubsystem();
	
	void TrackClock(TWeakObjectPtr<AObjectiveClock> InClock, bool bIsLeading);

	void SetClockSyncRequestDelegate(FSyncRequestDelegate& SyncDelegate);

	void OnSyncRequest(FRotator TargetRotation, float Tolerance);

	bool IsInteractable = false;

	void SetPlayerCharacter(TWeakObjectPtr<AMemoriesCharacter> MemoriesCharacter);

	bool bClocksFinished = true;

	bool bSpotlightsFinished = true;

	bool bFireBowlsFinished = true;

	bool bMovableFinished = true;

	void TryFinishGame();

	void ObjectiveFinished();

	void TrackSpotlight(TWeakObjectPtr<ALightSourceAndTargtetActor> Light);
	void FinishSpotLight();

	void TrackMovable(TWeakObjectPtr<AMovableActor> MovableActor);
	void FinishMovable();

	void TrackBasin(TWeakObjectPtr<ATorchBasin> Basin);
	void FinishBasin();

	FTimerHandle ObjectiveFinishedHandle;

	UFUNCTION(BlueprintCallable)
	bool CanPassClocks();
private:
	//Clock Begin
	TArray<TWeakObjectPtr<AObjectiveClock>> ListeningClocks;
	TWeakObjectPtr<AObjectiveClock> LeadingClock;

	TMap<EObjectiveType, bool> ObjectiveTracker;

	TWeakObjectPtr<AMemoriesCharacter> MemoriesCharacter;


	bool bGameFinished = false;
	
	TArray<TWeakObjectPtr<ALightSourceAndTargtetActor>> Spotlights;
	int32 SpotlightIndex = 0;
	TArray<TWeakObjectPtr<AMovableActor>> Movables;
	int32 MovableIndex = 0;
	TArray<TWeakObjectPtr<ATorchBasin>> Basins;
	int32 BasinIndex = 0;

	
	
	TObjectPtr<USoundCue> ClockFinishSound;

	TObjectPtr<USoundCue> OneObjectiveFinishedSound;
	//ClockEnd;
};
