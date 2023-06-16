// Fill out your copyright notice in the Description page of Project Settings.


#include "MemoriesPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "MemoriesCharacter.h"

void AMemoriesPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if(!IMC_Default_MK.IsNull())
		{
			Subsystem->AddMappingContext(IMC_Default_MK.Get(), 0);
		}
	}
}

void AMemoriesPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	if(!ensure(EnhancedInputComponent.IsValid())) return;

	EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &ThisClass::OnMoveInput);

	EnhancedInputComponent->BindAction(IA_Look, ETriggerEvent::Triggered, this, &ThisClass::OnLookInput);

	EnhancedInputComponent->BindAction(IA_Interact, ETriggerEvent::Triggered, this, &ThisClass::OnInteractInput);

	EnhancedInputComponent->BindAction(IA_Crouch, ETriggerEvent::Triggered, this, &ThisClass::OnCrouchInput);

	EnhancedInputComponent->BindAction(IA_Jump, ETriggerEvent::Started, this, &ThisClass::OnJumpBeginInput);
	EnhancedInputComponent->BindAction(IA_Jump, ETriggerEvent::Completed, this, &ThisClass::OnJumpEndInput);

	EnhancedInputComponent->BindAction(IA_Run, ETriggerEvent::Started, this, &ThisClass::OnRunBeginInput);
	EnhancedInputComponent->BindAction(IA_Run, ETriggerEvent::Completed, this, &ThisClass::OnRunEndInput);
}

void AMemoriesPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	MemoriesCharacter = Cast<AMemoriesCharacter>(GetCharacter());

	if(IsValid(PlayerCameraManager) && MemoriesCharacter.IsValid())
	{
		float Min;
		float Max;
		MemoriesCharacter->GetCameraPitchMinMax(Min, Max);
		PlayerCameraManager->ViewPitchMin = Min;
		PlayerCameraManager->ViewPitchMax = Max;
	}
}

void AMemoriesPlayerController::OnMoveInput(const FInputActionValue& Value)
{
	const FVector2D MoveValue = Value.Get<FVector2D>();
	if(MemoriesCharacter.IsValid())
	{
		const FRotator MovementRotation(0.0f, GetControlRotation().Yaw, 0.0f);
	
		if (MoveValue.X != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
			MemoriesCharacter->AddMovementInput(MovementDirection, MoveValue.X);
		}
		if (MoveValue.Y != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			MemoriesCharacter->AddMovementInput(MovementDirection, MoveValue.Y);
		}
	}
}

void AMemoriesPlayerController::OnLookInput(const FInputActionValue& Value)
{
	const FVector2D LookValue = Value.Get<FVector2D>();
	if (LookValue.X != 0.0f)
	{
		AddYawInput(LookValue.X); // * LookRate
	}

	if (LookValue.Y != 0.0f)
	{
		AddPitchInput(LookValue.Y); //* LookRate
	}
}

void AMemoriesPlayerController::OnRunBeginInput(const FInputActionValue& Value)
{
	if(MemoriesCharacter.IsValid())
	{
		MemoriesCharacter->TryStartRun();
	}
}

void AMemoriesPlayerController::OnRunEndInput(const FInputActionValue& Value)
{
	if(MemoriesCharacter.IsValid())
	{
		MemoriesCharacter->TryStopRun();
	}
}

void AMemoriesPlayerController::OnJumpBeginInput(const FInputActionValue& Value)
{
	if(MemoriesCharacter.IsValid())
	{
		MemoriesCharacter->Jump();
	}
}

void AMemoriesPlayerController::OnJumpEndInput(const FInputActionValue& Value)
{
	if(MemoriesCharacter.IsValid())
	{
		MemoriesCharacter->StopJumping();
	}
}

void AMemoriesPlayerController::OnInteractInput(const FInputActionValue& Value)
{
	if(MemoriesCharacter.IsValid())
	{
		MemoriesCharacter->TryInteract();
	}
}

void AMemoriesPlayerController::OnCrouchInput(const FInputActionValue& Value)
{
	if(MemoriesCharacter.IsValid())
	{
		MemoriesCharacter->TryCrouch();
	}
}
