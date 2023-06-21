// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectiveActors/ObjectiveClock.h"

#include "MemoriesCharacter.h"
#include "Project_Memories/Project_Memories.h"
#include "Subsystems/ObjectiveSubsystem.h"

constexpr float HourDegree = -360 / 12;
constexpr float MinuteDegreePerHour = -360;

// Sets default values
AObjectiveClock::AObjectiveClock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ClockBase = CreateDefaultSubobject<UStaticMeshComponent>("ClockBase");
	if(IsValid(ClockBase))
	{
		SetRootComponent(ClockBase);
		ClockBase->SetCollisionResponseToChannel(COLLISION_INTERACTABLE, ECR_Overlap);
	}
	HourDial = CreateDefaultSubobject<UStaticMeshComponent>("HourDial");
	if(IsValid(HourDial))
	{
		HourDial->SetupAttachment(RootComponent);
	}
	MinuteDial = CreateDefaultSubobject<UStaticMeshComponent>("MinuteDial");
	if(IsValid(MinuteDial))
	{
		MinuteDial->SetupAttachment(RootComponent);
	}
}

bool AObjectiveClock::IsAvailableForInteraction_Implementation(AActor* InteractingActor,
	UPrimitiveComponent* InteractionComponent) const
{
	if(bIsLeadingClock || bShouldFinish || Locked)
	{
		return false;
	}
	return true;
}

FInteractMessageInformation AObjectiveClock::GetInteractionMessageType_Implementation(
	UPrimitiveComponent* InteractionComponent) const
{
	if(bStopped)
	{
		return StartClockMessage;
	}
	else
	{
		return StopClockMessage;
	}
}

float AObjectiveClock::GetInteractionDuration_Implementation(UPrimitiveComponent* InteractionComponent) const
{
	return 0.0f;
}

void AObjectiveClock::PostInteract_Implementation(AActor* InteractingActor, UPrimitiveComponent* InteractionComponent)
{
	const TWeakObjectPtr<AMemoriesCharacter> Char = Cast<AMemoriesCharacter>(InteractingActor);
	if(Char.IsValid())
	{
		Char->ResetInteractionTrace();
	}
	if(bStopped)
	{
		bStopped = false;
	}
	else
	{
		bStopped = true;
	}
}

// Called when the game starts or when spawned
void AObjectiveClock::BeginPlay()
{
	Super::BeginPlay();

	const TWeakObjectPtr<UWorld> World = GetWorld();
	if(World.IsValid())
	{
		ObjectiveSubsystem = World->GetSubsystem<UObjectiveSubsystem>();
	}
	if(ObjectiveSubsystem.IsValid())
	{
		ObjectiveSubsystem->TrackClock(this, bIsLeadingClock);
		if(bIsLeadingClock)
		{
			ObjectiveSubsystem->SetClockSyncRequestDelegate(SyncRequestDelegate);
		}
	}
	HourDegreePerSecond = HourDegree / SecondsPerHour;
	
	MinuteDegreePerSecond = MinuteDegreePerHour / SecondsPerHour;

	SyncToleranceInDegree = SyncToleranceInSeconds * HourDegreePerSecond;
	SetStartRotation();
	bBeginPlayFinished = true;
}

void AObjectiveClock::SetStartRotation() const
{
	if(!IsValid(HourDial) || !IsValid(MinuteDial))
	{
		return;
	}
	const float Hours = FMath::Floor(StartTime);
	float Pitch = Hours * HourDegree;
	FRotator NewRotation = {Pitch,0,0};
	HourDial->AddLocalRotation(NewRotation);

	const float Minutes = (StartTime - Hours);
	Pitch = Minutes * MinuteDegreePerHour;
	NewRotation = {Pitch,0,0};
	MinuteDial->AddLocalRotation(NewRotation);
}

// Called every frame
void AObjectiveClock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bStopped)
	{
		return;
	}
	if(!bBeginPlayFinished)
	{
		return;
	}
	if(!IsValid(HourDial) || !IsValid(MinuteDial))
	{
		return;
	}
	if(!bShouldFinish)
	{
		float Pitch = DeltaTime * HourDegreePerSecond;
		FRotator NewRotation = {Pitch,0,0};
		HourDial->AddLocalRotation(NewRotation);

		Pitch = DeltaTime * MinuteDegreePerSecond;
		NewRotation = {Pitch,0,0};
		MinuteDial->AddLocalRotation(NewRotation);

		if(bIsLeadingClock && SyncRequestDelegate.IsBound())
		{
			SyncRequestDelegate.Execute(HourDial->GetComponentRotation(), FMath::Abs(SyncToleranceInDegree));
		}
	}
	else
	{
		if(CurrentTimeToFinishClock < 1.0f)
		{
			CurrentTimeToFinishClock += DeltaTime;
			const float Alpha = FMath::Clamp(CurrentTimeToFinishClock / 1.0f, 0, 1);
	
			HourDial->AddLocalRotation(FMath::Lerp(HourFinishStartRotation, FRotator::ZeroRotator, Alpha));
			MinuteDial->AddLocalRotation(FMath::Lerp(MinuteFinishStartRotation, FRotator::ZeroRotator, Alpha));
		}
		else
		{
			bStopped = true;
		}
	}

}

void AObjectiveClock::SetClockLocked()
{
	Locked = true;
}

bool AObjectiveClock::IsStopped()
{
	return bStopped;
}

void AObjectiveClock::StartFinishingClock()
{
	bShouldFinish = true;
	if(IsValid(HourDial) && IsValid(MinuteDial))
	{
		MinuteFinishStartRotation = MinuteDial->GetComponentRotation();
		HourFinishStartRotation = HourDial->GetComponentRotation();
	}
}

FRotator AObjectiveClock::GetHourHandRotation() const
{
	if(IsValid(HourDial))
	{
		return HourDial->GetComponentRotation();
	}
	else
	{
		return FRotator::ZeroRotator;
	}
}



