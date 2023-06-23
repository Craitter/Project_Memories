// Fill out your copyright notice in the Description page of Project Settings.


#include "MemoriesPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "MemoriesCharacter.h"
#include "InteractionComponent.h"
#include "Blueprint/UserWidget.h"
#include "Widgets/GameOverlayWidget.h"

void AMemoriesPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if(!IMC_Default_MK.IsNull())
		{
			Subsystem->AddMappingContext(IMC_Default_MK.LoadSynchronous(), 0);
		}
	}
	
	if(GameOverlayClass != nullptr)
	{
		GameOverlayWidget = CreateWidget<UGameOverlayWidget>(this ,GameOverlayClass);
		if(GameOverlayWidget != nullptr)
		{
			GameOverlayWidget->AddToViewport();
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

	EnhancedInputComponent->BindAction(IA_Interact, ETriggerEvent::Started, this, &ThisClass::OnInteractBeginInput);
	EnhancedInputComponent->BindAction(IA_Interact, ETriggerEvent::Completed, this, &ThisClass::OnInteractEndInput);

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
		const TWeakObjectPtr<UInteractionComponent> CharacterInteractionComponent = Cast<UInteractionComponent>(
			MemoriesCharacter->GetComponentByClass(UInteractionComponent::StaticClass()));
		if(!CharacterInteractionComponent.IsValid())
		{
			UE_LOG(LogTemp, Error , TEXT("%s() InteractionComponentInControllerNotValid!"), *FString(__FUNCTION__));
		}
		CharacterInteractionComponent->OnInteractableLost.BindUObject(this, &ThisClass::InteractableLost);
		CharacterInteractionComponent->OnNewInteractableFound.BindUObject(this, &ThisClass::NewInteractableFound);
		CharacterInteractionComponent->OnInteractionStarted.BindUObject(this, &ThisClass::InteractionStarted);
		CharacterInteractionComponent->OnInteractionFinished.BindUObject(this, &ThisClass::InteractionFinished);
	}
	
}

void AMemoriesPlayerController::NewInteractableFound(float InteractDuration, FInteractMessageInformation InteractMessageInformation)
{
	FFormatNamedArguments Args;
	if(InteractDuration > 0)
	{
		Args.Add(TEXT("HoldPress"), FText::FromString(TEXT("Hold")));
	}
	else
	{
		Args.Add(TEXT("HoldPress"), FText::FromString(TEXT("Press")));
	}
	Args.Add(TEXT("verb"), GetInteractableVerb(InteractMessageInformation.InteractMessageType));
	Args.Add(TEXT("name"), InteractMessageInformation.InteractableName);
	const FText OutText = FText::Format(InteractPromptBase, Args);
	if(GameOverlayWidget != nullptr)
	{
		GameOverlayWidget->ShowInteractionPrompt(InteractDuration, OutText);
	}
	//Todo: Update Widget
}

void AMemoriesPlayerController::InteractableLost()
{
	//Todo: UpdateWidget
	if(GameOverlayWidget != nullptr)
	{
		GameOverlayWidget->HideInteractionPrompt();
		GameOverlayWidget->StopInteractionTimer();
	}
}

void AMemoriesPlayerController::InteractionStarted(float InteractDuration)
{
	//TODO: Update Widget
	if(GameOverlayWidget != nullptr)
	{
		GameOverlayWidget->StartInteractionTimer(InteractDuration);
	}
}

void AMemoriesPlayerController::InteractionFinished()
{
	if(GameOverlayWidget != nullptr)
	{
		GameOverlayWidget->StopInteractionTimer();
	}
}

FText AMemoriesPlayerController::GetInteractableVerb(EInteractMessageType MessageInformation)
{
	FText OutText = FText::GetEmpty();
	switch (MessageInformation)
	{
		case EInteractMessageType::IMT_None:
			UE_LOG(LogTemp, Error , TEXT("%s() Shouldnt be called with none"), *FString(__FUNCTION__));
			break;
	case EInteractMessageType::IMT_Push:
			OutText = FText::FromString(TEXT("Push"));
			break;
		case EInteractMessageType::IMT_Rotate:
			OutText = FText::FromString(TEXT("Rotate"));
			break;
		case EInteractMessageType::IMT_Talk:
			OutText = FText::FromString(TEXT("Talk"));
			break;
		case EInteractMessageType::IMT_Start:
			OutText = FText::FromString(TEXT("Start"));
			break;
		case EInteractMessageType::IMT_Stop:
			OutText = FText::FromString(TEXT("Stop"));
			break;
	case EInteractMessageType::IMT_Ignite:
		OutText = FText::FromString(TEXT("Ignite"));
		break;
		default: ;
	}
	return OutText;
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

void AMemoriesPlayerController::OnInteractBeginInput(const FInputActionValue& Value)
{
	if(MemoriesCharacter.IsValid())
	{
		MemoriesCharacter->TryBeginInteract();
	}
}

void AMemoriesPlayerController::OnInteractEndInput(const FInputActionValue& Value)
{
	if(MemoriesCharacter.IsValid())
	{
		MemoriesCharacter->StopInteract();
	}
}

void AMemoriesPlayerController::OnCrouchInput(const FInputActionValue& Value)
{
	return;
	if(MemoriesCharacter.IsValid())
	{
		MemoriesCharacter->TryCrouch();
	}
}
