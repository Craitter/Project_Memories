// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Memories/public/MemoriesCharacter.h"

#include "InteractionComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
AMemoriesCharacter::AMemoriesCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	this->bUseControllerRotationYaw = false;
	this->bUseControllerRotationPitch = false;
	this->bUseControllerRotationRoll = false;
	
	const TWeakObjectPtr<UCharacterMovementComponent> MovementComponent = GetCharacterMovement();
	if(MovementComponent.IsValid())
	{
		InternMaxWalkSpeed = MovementComponent->MaxWalkSpeed;
		MovementComponent->NavAgentProps.bCanCrouch = true;
		MovementComponent->NavAgentProps.bCanJump = true;
		MovementComponent->NavAgentProps.bCanWalk = true;
		MovementComponent->NavAgentProps.bCanFly = false;
		MovementComponent->NavAgentProps.bCanSwim = false;

		//The Rate the Pawn Rotates to Fit the Controller Rotation
		MovementComponent->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

		MovementComponent->JumpZVelocity = 500.0f;
		
		MovementComponent->bUseControllerDesiredRotation = false;
		MovementComponent->bOrientRotationToMovement = true;


	}

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	if(IsValid(SpringArmComponent))
	{
		SpringArmComponent->bUsePawnControlRotation = true;
		SpringArmComponent->SetupAttachment(RootComponent);
	}
	CameraComponent = CreateDefaultSubobject<UCameraComponent>("Camera");
	if(IsValid(CameraComponent))
	{
		CameraComponent->SetupAttachment(SpringArmComponent);
	}
	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>("InteractionComponent");
	if(IsValid(InteractionComponent))
	{
		
	}
}

void AMemoriesCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	if(bTriesToStartRunning)
	{
		TryStartRun();
	}
}

// Called when the game starts or when spawned
void AMemoriesCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMemoriesCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(bShouldRotateToPlayerCharacter)
	{
		if(DialogueRotationTime > CurrentRotationTime)
		{
			CurrentRotationTime += DeltaTime;
			float Alpha = CurrentRotationTime / DialogueRotationTime;
			Alpha = FMath::Clamp(Alpha, 0, 1);
			const TWeakObjectPtr<UGameInstance> GameInstance = GetGameInstance();
			if(GameInstance.IsValid())
			{
				TWeakObjectPtr<APlayerController> PlayerController = GameInstance->GetPrimaryPlayerController();
				if(PlayerController.IsValid())
				{
					TWeakObjectPtr<AMemoriesCharacter> PlayerPawn = Cast<AMemoriesCharacter>(PlayerController->GetPawn());
					if(PlayerPawn.IsValid())
					{
						DialogueFacingTargetLocation.Z = 0.0f;
						FVector Temp = DialogueFacingTargetLocation - GetActorLocation();
			
						FRotator TargetRotation = Temp.ToOrientationRotator();
						// TargetRotation.Yaw -= 180.0f;
						const FRotator CurrentRotation = GetActorRotation();
						TargetRotation.Pitch = CurrentRotation.Pitch;
						TargetRotation.Roll = CurrentRotation.Roll;
						// TargetForward = TargetForward - CurrentRotation;
						SetActorRotation(FMath::Lerp(CurrentRotation, TargetRotation, Alpha));
					}
				}
			}
		}
		else
		{
			CurrentRotationTime = 0.0f;
			bShouldRotateToPlayerCharacter = false;
		}
	}

	if(bShouldRotateToInteractable)
	{
		if(DialogueRotationTime > CurrentRotationTime)
		{
			CurrentRotationTime += DeltaTime;
			float Alpha = CurrentRotationTime / DialogueRotationTime;
			Alpha = FMath::Clamp(Alpha, 0, 1);
			
			DialogueFacingTargetLocation.Z = 0.0f;
			FVector Temp = DialogueFacingTargetLocation - GetActorLocation();
			
			FRotator TargetRotation = Temp.ToOrientationRotator();
			
			
			
			const FRotator CurrentRotation = GetActorRotation();
			TargetRotation.Pitch = CurrentRotation.Pitch;
			TargetRotation.Roll = CurrentRotation.Roll;
			SetActorRotation(FMath::Lerp(CurrentRotation, TargetRotation, Alpha));
				
		}
		else
		{
			CurrentRotationTime = 0.0f;
			bShouldRotateToInteractable = false;
		}
	}

}


void AMemoriesCharacter::Jump()
{
	if(bIsCrouched)
	{
		UnCrouch();
	}
	Super::Jump();
}

void AMemoriesCharacter::TryStartRun()
{
	const TWeakObjectPtr<UCharacterMovementComponent> MovementComponent = GetCharacterMovement();
	if(MovementComponent.IsValid())
	{
		MovementComponent->UnCrouch();
		if(bIsCrouched)
		{
			bTriesToStartRunning = true;
			return;
		}
		bIsRunning = true;
		GetCharacterMovement()->MaxWalkSpeed = MaxRunSpeed;
	}
}

void AMemoriesCharacter::TryStopRun()
{
	const TWeakObjectPtr<UCharacterMovementComponent> MovementComponent = GetCharacterMovement();
	if(MovementComponent.IsValid())
	{
		bTriesToStartRunning = false;
		bIsRunning = false;
		GetCharacterMovement()->MaxWalkSpeed = InternMaxWalkSpeed;
	}
}

bool AMemoriesCharacter::IsRunning() const
{
	return bIsRunning;
}

void AMemoriesCharacter::TryCrouch()
{
	if(bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		if(IsRunning())
		{
			TryStopRun();
		}
		Crouch();
	}
}

void AMemoriesCharacter::TryBeginInteract()
{
	if(IsValid(InteractionComponent))
	{
		InteractionComponent->StartInteract();
	}
}

void AMemoriesCharacter::StopInteract()
{
	if(IsValid(InteractionComponent))
	{
		InteractionComponent->StopInteract();
	}
}

void AMemoriesCharacter::GetCameraPitchMinMax(float& Min, float& Max)
{
	Min = ViewPitchMin;
	Max = ViewPitchMax;
}

FVector AMemoriesCharacter::GetCameraForwardVector()
{
	if(IsValid(CameraComponent))
	{
		return CameraComponent->GetForwardVector();
	}
	return FVector::ZeroVector;
}

void AMemoriesCharacter::ResetInteractionTrace()
{
	if(IsValid(InteractionComponent))
	{
		InteractionComponent->ResetInteraction();
	}
}



