// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObjectiveClock.generated.h"

UCLASS()
class PROJECT_MEMORIES_API AObjectiveClock : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AObjectiveClock();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> ClockBase = {nullptr};

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> HourDial = {nullptr};

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> MinuteDial = {nullptr};

	UPROPERTY(EditInstanceOnly, meta = (ClampMin = "0.0", ClampMax = "23.99", ForceUnits = "Hour"))
	float StartTime = 0.0f;
	//this is the clock where everyone has to sync to so make sure its only one clock in the level
	UPROPERTY(EditInstanceOnly)
	bool bIsLeadingClock = false;

	bool bIsFirstTick = true;
	bool bStopped = false;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
