// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectiveActors/MovableActor.h"


#include "MemoriesCharacter.h"
#include "Engine/TriggerVolume.h"
#include "Project_Memories/Project_Memories.h"
#include "Subsystems/ObjectiveSubsystem.h"

// Sets default values
AMovableActor::AMovableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TargetObjective = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TargteObjective"));
	if(IsValid(TargetObjective))
	{
		SetRootComponent(TargetObjective);
		TargetObjective->SetCollisionResponseToChannel(COLLISION_INTERACTABLE, ECR_Overlap);
	}
}

bool AMovableActor::IsAvailableForInteraction_Implementation(AActor* InteractingActor,
	UPrimitiveComponent* InteractionComponent) const
{
	if(bObjectiveFinished || bIsMoving)
	{
		return false;
	}
	else
	{
		if(ObjectiveSubsystem.IsValid()) return ObjectiveSubsystem->IsInteractable;
		return false;
	}
}

FInteractMessageInformation AMovableActor::GetInteractionMessageType_Implementation(
	UPrimitiveComponent* InteractionComponent) const
{
	return MessageInfo;
}

float AMovableActor::GetInteractionDuration_Implementation(UPrimitiveComponent* InteractionComponent) const
{
	return InteractionTime;
}

void AMovableActor::PreInteract_Implementation(AActor* InteractingActor, UPrimitiveComponent* InteractionComponent)
{
	TimeOfLastInteraction = 0.0f;
}

void AMovableActor::PostInteract_Implementation(AActor* InteractingActor, UPrimitiveComponent* InteractionComponent)
{
	if(InteractingActor == nullptr)
	{
		return;
	}
	StartLocation = GetActorLocation();
	FVector ForwardVector = Cast<AMemoriesCharacter>(InteractingActor)->GetCameraForwardVector();
	ForwardVector.Z = 0.0f;
	NextLocation = StartLocation + (ForwardVector * TravelDistancePerInteraction);
	FHitResult Result;
	TargetObjective->SweepComponent(Result, StartLocation, NextLocation, TargetObjective->GetComponentRotation().Quaternion(), TargetObjective->GetCollisionShape());
#if ENABLE_DRAW_DEBUG
	if (bDebug)
	{
		const FCollisionShape TargetObjectiveShape = TargetObjective->GetCollisionShape(0);
		if(TargetObjectiveShape.IsSphere())
		{
			if (Result.bBlockingHit)
			{
				DrawDebugSphere(GetWorld(), NextLocation, TargetObjectiveShape.GetSphereRadius(), 10, FColor::Red, false, 1.0f);			}
			else
			{
				DrawDebugSphere(GetWorld(), NextLocation, TargetObjectiveShape.GetSphereRadius(), 10, FColor::Green, false, 1.0f);			}
			
		}
		else if(TargetObjectiveShape.IsBox())
		{
			if (Result.bBlockingHit)
			{
				DrawDebugBox(GetWorld(), NextLocation, TargetObjectiveShape.GetExtent(), FColor::Red, false, 1.0f);
			}
			else
			{
				DrawDebugBox(GetWorld(), NextLocation, TargetObjectiveShape.GetExtent(), FColor::Green, false, 1.0f);
			}
		}
		else
		{
			if (Result.bBlockingHit)
			{
				DrawDebugCapsule(GetWorld(), NextLocation, TargetObjectiveShape.GetCapsuleHalfHeight(), TargetObjectiveShape.GetCapsuleRadius(), TargetObjective->GetComponentRotation().Quaternion(), FColor::Red, false, 1.0f);
			}
			else
			{
				DrawDebugCapsule(GetWorld(), NextLocation, TargetObjectiveShape.GetCapsuleHalfHeight(), TargetObjectiveShape.GetCapsuleRadius(), TargetObjective->GetComponentRotation().Quaternion(), FColor::Green, false, 1.0f);
			}
		}
		
	}
#endif // ENABLE_DRAW_DEBUG	
	bIsMoving = !Result.bBlockingHit;
}

bool AMovableActor::HasInteractionStateChanged_Implementation(UPrimitiveComponent* InteractionComponent)
{
	if(bIsReadyForInteractionAgain)
	{
		bIsReadyForInteractionAgain = false;
		return true;
	}
	return false;
}

void AMovableActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(bFromSweep)
	{
		UE_LOG(LogTemp, Warning , TEXT("%s %s() sweep overlap return"), *UEnum::GetValueAsString(GetLocalRole()), *FString(__FUNCTION__));
		return;
	}
	if(Cast<ATriggerVolume>(OtherActor) == TargetTriggerVolume && TargetTriggerVolume != nullptr)
	{
		
		CurrentTravelTime = 0.0f;
		TimeOfLastInteraction = 0.0f;
		NextLocation = OverrideTargetLocation;
		if(NextLocation == FVector::ZeroVector)
		{
			NextLocation = TargetTriggerVolume->GetActorLocation();
			bIsMoving = true;
		}
		StartLocation = GetActorLocation();
	} 
}


// Called when the game starts or when spawned
void AMovableActor::BeginPlay()
{
	Super::BeginPlay();

	if(TargetObjective != nullptr)
	{
		TargetObjective->OnComponentBeginOverlap.AddDynamic(this, &AMovableActor::OnOverlapBegin);
	}
	MovesDone.Add(GetActorLocation());
	ObjectiveSubsystem = GetWorld()->GetSubsystem<UObjectiveSubsystem>();
	// if(ObjectiveSubsystem.IsValid())
	// {
	// 	ObjectiveSubsystem->TrackMovable(this);
	// }
}

// Called every frame
void AMovableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
#if ENABLE_DRAW_DEBUG
	if (bDebug)
	{
		if(IsValid(TargetTriggerVolume))
		{
			
			DrawDebugSphere(GetWorld(), TargetTriggerVolume->GetActorLocation(), 50, 10, FColor::Blue, false);
		}
	}
#endif // ENABLE_DRAW_DEBUG	
	if(!bObjectiveFinished)
	{
		if(bIsMoving)
		{
			TimeOfLastInteraction = 0.0f;
			if(TimeToTravel > CurrentTravelTime)
			{
				CurrentTravelTime += DeltaTime;
				
				const float TimeRatio = FMath::Clamp(CurrentTravelTime / TimeToTravel, 0, 1);
				const float MoveAlpha = MoveCurve.GetRichCurveConst()->Eval(TimeRatio);
				SetActorLocation(FMath::Lerp(StartLocation, NextLocation, MoveAlpha));
			}
			else
			{
				if(!bIsForced)
				{
					MovesDone.Add(GetActorLocation());
				}
				else
				{
					TimeOfLastInteraction = RewindTime - ChainedRewindTime;
				}
				CurrentTravelTime = 0.0f;
				bIsMoving = false;
				bIsReadyForInteractionAgain = true;
				if(TargetTriggerVolume == nullptr)
				{
					UE_LOG(LogTemp, Warning , TEXT("%s %s() NO TARGET TRIGGER VOLUME"), *UEnum::GetValueAsString(GetLocalRole()), *FString(__FUNCTION__));
				}
				else if(GetActorLocation().Equals(OverrideTargetLocation) || GetActorLocation().Equals(TargetTriggerVolume->GetActorLocation()))
				{
					bObjectiveFinished = true;
					// if(ObjectiveSubsystem.IsValid())
					// {
					// 	ObjectiveSubsystem->FinishMovable();
					// }
				}
			}	
		}
		else
		{
			TimeOfLastInteraction += DeltaTime;
			if(RewindTime < TimeOfLastInteraction)
			{
				if(MovesDone.Num() > 1)
				{
					StartLocation = GetActorLocation();
					MovesDone.RemoveAt(MovesDone.Num() - 1);
					NextLocation = MovesDone.Last();
					bIsMoving = true;
					bIsForced = true;
				}
			}
		}
	}
}

