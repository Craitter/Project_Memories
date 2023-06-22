// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Memories/public/MemoriesCharacter.h"

#include "InteractionComponent.h"
#include "NiagaraComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/AudioComponent.h"
#include "Components/MounteaDialogueParticipant.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ObjectiveActors/FastSwappingTextActor.h"
#include "Project_Memories/Project_Memories.h"
#include "Sound/SoundCue.h"
#include "Subsystems/ObjectiveSubsystem.h"

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

	Torch = CreateDefaultSubobject<UStaticMeshComponent>("Torch");
	if(IsValid(Torch))
	{
		Torch->SetupAttachment(GetMesh(), "righthandsocket");
	}

	TorchDecoration = CreateDefaultSubobject<UStaticMeshComponent>("TorchDecoration");
	if(IsValid(TorchDecoration))
	{
		TorchDecoration->SetupAttachment(Torch);
	}

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("FireEffect");
	if(IsValid(NiagaraComponent))
	{
		NiagaraComponent->SetupAttachment(Torch);
	}

	FootStep = CreateDefaultSubobject<UAudioComponent>("DefaultGameSound");
	if(IsValid(FootStep))
	{
		FootStep->SetAutoActivate(false);
		FootStep->SetupAttachment(GetMesh());
	}

	
	ClockTicking = CreateDefaultSubobject<UAudioComponent>("ClockTicking");
	if(IsValid(ClockTicking))
	{
		// ClockTicking->SetAutoActivate(false);
		ClockTicking->SetupAttachment(GetMesh());
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
	const TWeakObjectPtr<UCharacterMovementComponent> MovementComponent = GetCharacterMovement();
	if(MovementComponent.IsValid())
	{
		InternMaxWalkSpeed = MovementComponent->MaxWalkSpeed;
	}
	if(IsValid(NiagaraComponent) && IsValid(Torch) && IsValid(TorchDecoration))
	{
		TorchDecoration->SetVisibility(false);
		Torch->SetVisibility(false);
		NiagaraComponent->Deactivate();
	}
	const TWeakObjectPtr<USkeletalMeshComponent> TEmpMesh = GetMesh();
	if(TEmpMesh.IsValid())
	{
		TEmpMesh->SetCollisionResponseToChannel(COLLISION_INTERACTABLE, ECR_Overlap);
	}
	if(IsPlayerControlled())
	{
		GetWorld()->GetSubsystem<UObjectiveSubsystem>()->SetPlayerCharacter(this);
	}
	else
	{
		bPlaySadAnimation = true;
	}
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
	if(FootStep != nullptr)
	{
		if(FootStep->IsPlaying())
		{
			FVector VelocityIn = GetVelocity();
			VelocityIn.Z = 0.0f;
			if(VelocityIn.Length() < 10.0f)
			{
				FootStep->SetPaused(true);
			}
		}
		else
		{
			FVector VelocityIn = GetVelocity();
			VelocityIn.Z = 0.0f;
			if(VelocityIn.Length() >= 10.0f)
			{
				FootStep->Play();
			}
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

void AMemoriesCharacter::ClearFastText()
{
	if(TextToVanishAfterDialogue != nullptr)
	{
		TextToVanishAfterDialogue->Destroy();
	}
}


void AMemoriesCharacter::StopClockTicking()
{
	if(IsValid(ClockTicking))
	{
		ClockTicking->Stop();
	}
}

void AMemoriesCharacter::StopMusic()
{
	if(PlayingSound != nullptr)
	{
		PlayingSound->Stop();
		PlayingSound->DestroyComponent();
		
	}
}

void AMemoriesCharacter::EnableTorch()
{
	bHasTorch = true;
	if(IsValid(NiagaraComponent) && IsValid(Torch) && IsValid(TorchDecoration))
	{
		TorchDecoration->SetVisibility(true);
		Torch->SetVisibility(true);
		NiagaraComponent->Activate();
		// TArray<AActor*> Actors;
		// UGameplayStatics::GetAllActorsOfClass(this, this->StaticClass(), Actors);
		// for (const auto Actor : Actors)
		// {
		// 	TWeakObjectPtr<AMemoriesCharacter> Character = Cast<AMemoriesCharacter>(Actor);
		// 	if(Character.IsValid() && !Character->IsPlayerControlled())
		// 	{
		// 		Character->bIsInteractable = false;
		// 	}
		// }
		//
		// UMounteaDialogueParticipant* Participant = FindComponentByClass<UMounteaDialogueParticipant>();
		// if(IsValid(Participant))
		// {
		// 	Participant->SetParticipantState(EDialogueParticipantState::EDPS_Disabled);
		// }
	}

	const TWeakObjectPtr<UObjectiveSubsystem> ObjectiveSubsystem = GetWorld()->GetSubsystem<UObjectiveSubsystem>();
	if(ObjectiveSubsystem.IsValid())
	{
		ObjectiveSubsystem->IsInteractable = true;
	}
}

bool AMemoriesCharacter::IsTorchEnabled() const
{
	return bHasTorch;
}



