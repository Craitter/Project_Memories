// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/ObjectiveSubsystem.h"

#include "MemoriesCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

UObjectiveSubsystem::UObjectiveSubsystem()
{
	static ConstructorHelpers::FObjectFinder<USoundCue> ClockBPSound(TEXT("/Game/SFX/Clock"));
	ClockFinishSound = ClockBPSound.Object;
	
	static ConstructorHelpers::FObjectFinder<USoundCue> ObjectiveBPSound(TEXT("/Game/SFX/Stinger"));
	OneObjectiveFinishedSound = ObjectiveBPSound.Object; 
}

void UObjectiveSubsystem::TrackClock(TWeakObjectPtr<AObjectiveClock> InClock, bool bIsLeading)
{
	if(bIsLeading)
	{
		LeadingClock = InClock;
	}
	else
	{
		ListeningClocks.AddUnique(InClock);
	}
	bClocksFinished = false;
}

void UObjectiveSubsystem::SetClockSyncRequestDelegate(FSyncRequestDelegate& SyncDelegate)
{
	SyncDelegate.BindUObject(this, &UObjectiveSubsystem::OnSyncRequest);
}

void UObjectiveSubsystem::OnSyncRequest(FRotator TargetRotation, float Tolerance)
{
	
	bool bAllInSync = true;
	for(TWeakObjectPtr<AObjectiveClock> InClock : ListeningClocks)
	{
		if(InClock.IsValid())
		{
			if(InClock->GetHourHandRotation().Equals(TargetRotation, Tolerance) && !InClock->IsStopped())
			{
				
				if(!InClock->Locked)
				{
					InClock->SetClockLocked();
					UGameplayStatics::PlaySound2D(this, ClockFinishSound);
				}
			}
			else
			{
				bAllInSync = false;
			}
		}
	}
	if(bAllInSync)
	{
		
		for(TWeakObjectPtr<AObjectiveClock> InClock : ListeningClocks)
		{
			if(InClock.IsValid())
			{
				InClock->StartFinishingClock();
			}
		}
		if(LeadingClock.IsValid())
		{
			LeadingClock->StartFinishingClock();
		}
		if(MemoriesCharacter.IsValid())
		{
			MemoriesCharacter->StopClockTicking();
		}
		bClocksFinished = true;
		TryFinishGame();
	}
}

void UObjectiveSubsystem::SetPlayerCharacter(TWeakObjectPtr<AMemoriesCharacter> InMemoriesCharacter)
{
	MemoriesCharacter = InMemoriesCharacter;	
}

void UObjectiveSubsystem::TryFinishGame()
{
	ObjectiveFinished();
	if(bClocksFinished && bSpotlightsFinished && bMovableFinished && bFireBowlsFinished)
	{
		UE_LOG(LogTemp, Warning , TEXT("%s() GameFInished"), *FString(__FUNCTION__));
		//Finish Game
	}
}

void UObjectiveSubsystem::ObjectiveFinished()
{
	UGameplayStatics::PlaySound2D(this, OneObjectiveFinishedSound);
}

void UObjectiveSubsystem::TrackSpotlight(TWeakObjectPtr<ALightSourceAndTargtetActor> Light)
{
	Spotlights.AddUnique(Light);
	bSpotlightsFinished = false;
	
}

void UObjectiveSubsystem::FinishSpotLight()
{
	SpotlightIndex++;
	UGameplayStatics::PlaySound2D(this, ClockFinishSound);
	if(SpotlightIndex == Spotlights.Num())
	{
		bSpotlightsFinished = true;
		TryFinishGame();
	}
}

void UObjectiveSubsystem::TrackMovable(TWeakObjectPtr<AMovableActor> MovableActor)
{
	Movables.AddUnique(MovableActor);
	bMovableFinished = false;
}

void UObjectiveSubsystem::FinishMovable()
{
	MovableIndex++;
	UGameplayStatics::PlaySound2D(this, ClockFinishSound);
	if(Movables.Num() == MovableIndex)
	{
		bMovableFinished = true;
		TryFinishGame();
	}
}

void UObjectiveSubsystem::TrackBasin(TWeakObjectPtr<ATorchBasin> Basin)
{
	Basins.AddUnique(Basin);
	bFireBowlsFinished = false;
}

void UObjectiveSubsystem::FinishBasin()
{
	BasinIndex++;
	UGameplayStatics::PlaySound2D(this, ClockFinishSound);
	if(BasinIndex == Basins.Num())
	{
		bFireBowlsFinished = true;
		TryFinishGame();
	}
}
