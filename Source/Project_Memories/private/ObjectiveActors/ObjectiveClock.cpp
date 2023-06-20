// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectiveActors/ObjectiveClock.h"

// Sets default values
AObjectiveClock::AObjectiveClock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ClockBase = CreateDefaultSubobject<UStaticMeshComponent>("ClockBase");
	if(IsValid(ClockBase))
	{
		SetRootComponent(ClockBase);
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

// Called when the game starts or when spawned
void AObjectiveClock::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AObjectiveClock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bIsFirstTick)
	{
		bIsFirstTick = false;
		if(HourDial == nullptr || MinuteDial == nullptr)
		{
			return;
		}
		
	}
}

