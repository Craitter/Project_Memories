// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableInterface.h"
#include "GameFramework/Actor.h"
#include "MovableActor.generated.h"

class UObjectiveSubsystem;
class ATriggerVolume;

UCLASS()
class PROJECT_MEMORIES_API AMovableActor : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMovableActor();

	virtual bool IsAvailableForInteraction_Implementation(AActor* InteractingActor, UPrimitiveComponent* InteractionComponent) const override;


	virtual FInteractMessageInformation GetInteractionMessageType_Implementation(UPrimitiveComponent* InteractionComponent) const override;
	
	virtual float GetInteractionDuration_Implementation(UPrimitiveComponent* InteractionComponent) const override;

	virtual void PreInteract_Implementation(AActor* InteractingActor, UPrimitiveComponent* InteractionComponent) override;
	
	virtual void PostInteract_Implementation(AActor* InteractingActor, UPrimitiveComponent* InteractionComponent) override;

	virtual bool HasInteractionStateChanged_Implementation(UPrimitiveComponent* InteractionComponent) override;
	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// UPROPERTY(EditInstanceOnly, Category = "Target")
	// FVector TargetLocation = FVector::ZeroVector;

	UPROPERTY(EditInstanceOnly, Category = "Target")
	TObjectPtr<ATriggerVolume> TargetTriggerVolume = {nullptr};

	//leave empty if you want to use the center of the trigger box, change if you experience bugs using the trigger center
	UPROPERTY(EditInstanceOnly, Category = "Target")
	FVector OverrideTargetLocation = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, Category = "Target")
	TObjectPtr<UStaticMeshComponent> TargetObjective = {nullptr};

	UPROPERTY(EditAnywhere, Category = "Target")
	float TravelDistancePerInteraction;

	UPROPERTY(EditAnywhere, Category = "Target")
	float TimeToTravel = 3.0f;
	
	UPROPERTY(EditAnywhere, Category = "Target")
	float InteractionTime = 2.0f;

	//after x seconds the moves get rewinded, one at a time -1 = no rewind (Careful wwe have no pull mechanic so we can get stuck in a corner)
	UPROPERTY(EditAnywhere, Category = "Target")
	float RewindTime = 10.0f;

	//if one rewind happend this time can be the new Rewind time so we can lose progress faster if we stop doing continous
	UPROPERTY(EditAnywhere, Category = "Target")
	float ChainedRewindTime = 5.0f;

	UPROPERTY(EditInstanceOnly, Category = "Target")
	bool bObjectiveFinished = false;

	UPROPERTY(EditInstanceOnly, Category = "Target")
	bool bDebug = false;
	
	UPROPERTY(EditAnywhere, Category = "Target")
	FRuntimeFloatCurve MoveCurve;

	UPROPERTY(EditAnywhere, Category = "Target")
	FInteractMessageInformation MessageInfo;

	bool bIsForced = false;
	bool bIsMoving = false;
	float TimeOfLastInteraction = 0.0f;
	float CurrentTravelTime = 0.0f;
	FVector NextLocation = FVector::ZeroVector;
	TArray<FVector> MovesDone;
	FVector StartLocation = FVector::ZeroVector;
	bool bIsReadyForInteractionAgain = false;

private:
	TWeakObjectPtr<UObjectiveSubsystem> ObjectiveSubsystem = {nullptr};

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
