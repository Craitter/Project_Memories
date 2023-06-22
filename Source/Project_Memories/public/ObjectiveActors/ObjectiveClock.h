// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableInterface.h"
#include "GameFramework/Actor.h"
#include "ObjectiveClock.generated.h"


class UObjectiveSubsystem;
DECLARE_DELEGATE_TwoParams(FSyncRequestDelegate, FRotator, float)

UCLASS()
class PROJECT_MEMORIES_API AObjectiveClock : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AObjectiveClock();

	virtual bool IsAvailableForInteraction_Implementation(AActor* InteractingActor, UPrimitiveComponent* InteractionComponent) const override;

	virtual FInteractMessageInformation GetInteractionMessageType_Implementation(UPrimitiveComponent* InteractionComponent) const override;

	virtual float GetInteractionDuration_Implementation(UPrimitiveComponent* InteractionComponent) const override;

	virtual void PostInteract_Implementation(AActor* InteractingActor, UPrimitiveComponent* InteractionComponent) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Clock")
	TObjectPtr<UStaticMeshComponent> ClockBase = {nullptr};

	UPROPERTY(VisibleAnywhere, Category = "Clock")
	TObjectPtr<UStaticMeshComponent> HourDial = {nullptr};

	UPROPERTY(VisibleAnywhere, Category = "Clock")
	TObjectPtr<UStaticMeshComponent> MinuteDial = {nullptr};

	UPROPERTY(EditInstanceOnly, meta = (ClampMin = "0.0", ClampMax = "11.99", ForceUnits = "Hour"), Category = "Clock")
	float StartTime = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Clock")
	FInteractMessageInformation StopClockMessage;

	UPROPERTY(EditDefaultsOnly, Category = "Clock")
	FInteractMessageInformation StartClockMessage;

	UPROPERTY(EditDefaultsOnly, Category = "Clock" )
	float SyncToleranceInSeconds = 0.5f;
	
	//this defines the speed of the clocks
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.0", ClampMax = "3600.0", ForceUnits = "Sec"), Category = "Clock")
	float SecondsPerHour = 1.0f;
	//this is the clock where everyone has to sync to so make sure its only one clock in the level
	UPROPERTY(EditInstanceOnly, Category = "Clock")
	bool bIsLeadingClock = false;

	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool Locked = false;
	void SetClockLocked();
	bool IsStopped();

	void StartFinishingClock();

	FRotator GetHourHandRotation() const;

	FSyncRequestDelegate SyncRequestDelegate;
private:
	void SetStartRotation() const;

	TWeakObjectPtr<UObjectiveSubsystem> ObjectiveSubsystem = {nullptr};

	FRotator MinuteFinishStartRotation = FRotator::ZeroRotator;
	FRotator HourFinishStartRotation = FRotator::ZeroRotator;
	bool bShouldFinish = false;
	bool bBeginPlayFinished = false;
	bool bStopped = false;

	
	float CurrentTimeToFinishClock = 0.0f;
	float HourDegreePerSecond = 0.0f;
	float MinuteDegreePerSecond = 0.0f;
	
	float SyncToleranceInDegree;
	
};
