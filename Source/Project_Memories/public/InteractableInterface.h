// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableInterface.generated.h"


DECLARE_MULTICAST_DELEGATE(FOnInteractionFinishedDelegate);

UENUM(BlueprintType)
enum class EInteractMessageType : uint8
{
	IMT_None = 0			UMETA(Hidden),
	
	IMT_Push = 1			UMETA(DisplayName = "Push"),
 
	IMT_Rotate = 2			UMETA(DisplayName = "Rotate"),

	IMT_Talk = 4			UMETA(DisplayName = "Talk"),

	IMT_Stop = 5			UMETA(DisplayName = "Stop"),

	IMT_Start = 6			UMETA(DisplayName = "Start"),

	IMT_Ignite = 7			UMETA(DisplayName = "Ignite"),
	
};

USTRUCT(BlueprintType)
struct FInteractMessageInformation
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	EInteractMessageType InteractMessageType = EInteractMessageType::IMT_None;

	UPROPERTY(EditDefaultsOnly)
	FText InteractableName = FText::GetEmpty();
};
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECT_MEMORIES_API IInteractableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	bool IsAvailableForInteraction(AActor* InteractingActor, UPrimitiveComponent* InteractionComponent) const;
	virtual bool IsAvailableForInteraction_Implementation(AActor* InteractingActor, UPrimitiveComponent* InteractionComponent) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	FInteractMessageInformation GetInteractionMessageType(UPrimitiveComponent* InteractionComponent) const;
	virtual FInteractMessageInformation GetInteractionMessageType_Implementation(UPrimitiveComponent* InteractionComponent) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	float GetInteractionDuration(UPrimitiveComponent* InteractionComponent) const;
	virtual float GetInteractionDuration_Implementation(UPrimitiveComponent* InteractionComponent) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void PreInteract(AActor* InteractingActor, UPrimitiveComponent* InteractionComponent);
	virtual void PreInteract_Implementation(AActor* InteractingActor, UPrimitiveComponent* InteractionComponent) {};

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void PostInteract(AActor* InteractingActor, UPrimitiveComponent* InteractionComponent);
	virtual void PostInteract_Implementation(AActor* InteractingActor, UPrimitiveComponent* InteractionComponent) {};

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void CancelInteraction(UPrimitiveComponent* InteractionComponent);
	virtual void CancelInteraction_Implementation(UPrimitiveComponent* InteractionComponent) {};

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	bool HasInteractionStateChanged(UPrimitiveComponent* InteractionComponent);
	virtual bool HasInteractionStateChanged_Implementation(UPrimitiveComponent* InteractionComponent);
	
};
