// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableBaseActor.h"

// Sets default values
AInteractableBaseActor::AInteractableBaseActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

bool AInteractableBaseActor::IsAvailableForInteraction_Implementation(UPrimitiveComponent* InteractionComponent) const
{
	return true;
}

FInteractMessageInformation AInteractableBaseActor::GetInteractionMessageType(
	UPrimitiveComponent* InteractionComponent) const
{
	return MessageInformation;
}

float AInteractableBaseActor::GetInteractionDuration_Implementation(UPrimitiveComponent* InteractionComponent) const
{
	return InteractDuration;
}

void AInteractableBaseActor::PreInteract_Implementation(AActor* InteractingActor,
	UPrimitiveComponent* InteractionComponent)
{
	UE_LOG(LogTemp, Warning , TEXT("%s %s() PreInteract called"), *UEnum::GetValueAsString(GetLocalRole()), *FString(__FUNCTION__));
}

void AInteractableBaseActor::PostInteract_Implementation(AActor* InteractingActor,
	UPrimitiveComponent* InteractionComponent)
{
	
	UE_LOG(LogTemp, Warning , TEXT("%s %s() PostInteract called"), *UEnum::GetValueAsString(GetLocalRole()), *FString(__FUNCTION__));
	if(bDestroyAfter)
	{
		Destroy();
	}
}

void AInteractableBaseActor::CancelInteraction_Implementation(UPrimitiveComponent* InteractionComponent)
{
}

// Called when the game starts or when spawned
void AInteractableBaseActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInteractableBaseActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

