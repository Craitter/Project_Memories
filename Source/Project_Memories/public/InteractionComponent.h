// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"


struct  FInteractMessageInformation;
class AMemoriesCharacter;

DECLARE_DELEGATE_OneParam(FOnInteractableFound, FHitResult)

DECLARE_DELEGATE(FOnInteractableLost)
DECLARE_DELEGATE_TwoParams(FOnNewInteractableFound, float, FInteractMessageInformation)
DECLARE_DELEGATE_OneParam(FOnInteractionStarted, float)
DECLARE_DELEGATE(FOnInteractionFinished)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_MEMORIES_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "InteractionTraceCollisionProfile")
	FCollisionProfileName CollisionProfileName;
	//Draws debug lines wherever the trace goes
	UPROPERTY(EditDefaultsOnly, Category = "InteractionTraceCollisionProfile")
	bool bDebug = false;
	//in Seconds
	UPROPERTY(EditDefaultsOnly, Category = "InteractionTraceCollisionProfile")
	float InteractionLoopTime = 0.1f;

	//in cm
	UPROPERTY(EditDefaultsOnly, Category = "InteractionTraceCollisionProfile")
	float InteractionRange = 250.0f;
	//in cm
	UPROPERTY(EditDefaultsOnly, Category = "InteractionTraceCollisionProfile")
	float InteractionSphereRadius = 50.0f;
	FOnInteractableFound OnInteractableFound;
	void InteractableFound(FHitResult InteractableHit);
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void PerformTrace();

	FOnInteractableLost OnInteractableLost;
	FOnNewInteractableFound OnNewInteractableFound;
	FOnInteractionStarted OnInteractionStarted;
	FOnInteractionFinished OnInteractionFinished;

	void StartInteract();

	void StopInteract();

private:
	FTimerHandle InteractionTraceTimerHandle;
	FTimerHandle InteractDurationTimerHandle;

	TWeakObjectPtr<AMemoriesCharacter> OwningCharacter = {nullptr};

	FHitResult CachedHit;

	bool bHasInteractableTarget = false;
	bool bIsInteracting = false;

	void FinishInteract();
};


