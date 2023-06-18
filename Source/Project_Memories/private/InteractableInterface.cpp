// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableInterface.h"

// Add default functionality here for any IInteractableInterface functions that are not pure virtual.
bool IInteractableInterface::IsAvailableForInteraction_Implementation(AActor* InteractingActor, UPrimitiveComponent* InteractionComponent) const
{
	return false;
}

FInteractMessageInformation IInteractableInterface::GetInteractionMessageType_Implementation(
	UPrimitiveComponent* InteractionComponent) const
{
	return FInteractMessageInformation();
}

float IInteractableInterface::GetInteractionDuration_Implementation(UPrimitiveComponent* InteractionComponent) const
{
	return 0.0f;
}
