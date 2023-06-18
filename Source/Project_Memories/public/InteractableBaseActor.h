// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableInterface.h"
#include "GameFramework/Actor.h"
#include "InteractableBaseActor.generated.h"

UCLASS()
class PROJECT_MEMORIES_API AInteractableBaseActor : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractableBaseActor();
	virtual bool IsAvailableForInteraction_Implementation(UPrimitiveComponent* InteractionComponent) const override;
	virtual FInteractMessageInformation
	GetInteractionMessageType(UPrimitiveComponent* InteractionComponent) const override;
	virtual float GetInteractionDuration_Implementation(UPrimitiveComponent* InteractionComponent) const override;
	virtual void
	PreInteract_Implementation(AActor* InteractingActor, UPrimitiveComponent* InteractionComponent) override;
	virtual void
	PostInteract_Implementation(AActor* InteractingActor, UPrimitiveComponent* InteractionComponent) override;
	virtual void CancelInteraction_Implementation(UPrimitiveComponent* InteractionComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	float InteractDuration = 0.0f;

	UPROPERTY(EditDefaultsOnly)
	bool bDestroyAfter = false;
	UPROPERTY(EditDefaultsOnly)
	FInteractMessageInformation MessageInformation;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
