// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MemoriesPlayerController.generated.h"

struct FInputActionValue;
class AMemoriesCharacter;
class UInputMappingContext;
class UInputAction;
class UEnhancedInputComponent;
/**
 * 
 */
UCLASS()
class PROJECT_MEMORIES_API AMemoriesPlayerController : public APlayerController
{
	GENERATED_BODY()



protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

	virtual void OnPossess(APawn* InPawn) override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Move = {nullptr};

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Look = {nullptr};

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Jump = {nullptr};

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Run = {nullptr};
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Interact = {nullptr};

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Crouch = {nullptr};

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TSoftObjectPtr<UInputMappingContext> IMC_Default_MK = {nullptr};

private:
	void OnMoveInput(const FInputActionValue& Value);

	void OnLookInput(const FInputActionValue& Value);

	void OnRunBeginInput(const FInputActionValue& Value);

	void OnRunEndInput(const FInputActionValue& Value);

	void OnJumpBeginInput(const FInputActionValue& Value);
	
	void OnJumpEndInput(const FInputActionValue& Value);

	void OnInteractInput(const FInputActionValue& Value);

	void OnCrouchInput(const FInputActionValue& Value);

	


	TWeakObjectPtr<UEnhancedInputComponent> EnhancedInputComponent = {nullptr};

	TWeakObjectPtr<AMemoriesCharacter> MemoriesCharacter = {nullptr};
};
