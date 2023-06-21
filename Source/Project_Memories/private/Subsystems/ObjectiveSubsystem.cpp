// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/ObjectiveSubsystem.h"

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
				InClock->SetClockLocked();
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
	}
}
