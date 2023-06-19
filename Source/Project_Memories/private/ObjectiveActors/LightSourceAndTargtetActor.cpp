// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectiveActors/LightSourceAndTargtetActor.h"

#include "MemoriesCharacter.h"
#include "Components/SpotLightComponent.h"
#include "Engine/TriggerVolume.h"
#include "ObjectiveActors/RedirectActor.h"
#include "Project_Memories/Project_Memories.h"

// Sets default values
ALightSourceAndTargtetActor::ALightSourceAndTargtetActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	RotatingMesh = CreateDefaultSubobject<UStaticMeshComponent>("RotatingMesh");
	if(IsValid(RotatingMesh))
	{
		SetRootComponent(RotatingMesh);
		RotatingMesh->Mobility = EComponentMobility::Movable;
		RotatingMesh->SetCollisionResponseToChannel(COLLISION_INTERACTABLE, ECR_Overlap);
	}
	SpotLightComponent = CreateDefaultSubobject<USpotLightComponent>("SpotLight");
	if(IsValid(SpotLightComponent))
	{
		SpotLightComponent->SetupAttachment(RotatingMesh);
		SpotLightComponent->bCastDeepShadow = false;
		SpotLightComponent->SetCastShadows(false);
	}
	
}

bool ALightSourceAndTargtetActor::IsAvailableForInteraction_Implementation(AActor* InteractingActor,
	UPrimitiveComponent* InteractionComponent) const
{
	return !bIsFinished;
}

void ALightSourceAndTargtetActor::PostInteract_Implementation(AActor* InteractingActor,
	UPrimitiveComponent* InteractionComponent)
{
	if(AMemoriesCharacter* Player = Cast<AMemoriesCharacter>(InteractingActor))
	{
		StartRotation = SpotLightComponent->GetComponentRotation();
		Player->bShouldRotateToInteractable = true;
		Player->DialogueFacingTargetLocation = GetActorLocation();
		FVector Temp = Player->GetCameraForwardVector();
		Temp.Z = 0.0f;
		
		NextRotation = Temp.Rotation();
		NextRotation.Roll = StartRotation.Roll;
		NextRotation.Pitch = StartRotation.Pitch;
		bRotate = true;
		DeactivateLight();
	}
}

// Called when the game starts or when spawned
void ALightSourceAndTargtetActor::BeginPlay()
{
	Super::BeginPlay();

	if(TargetTriggerVolume != nullptr)
	{
		const TWeakObjectPtr<UPrimitiveComponent> PrimitiveComponent = Cast<UPrimitiveComponent>(TargetTriggerVolume->GetRootComponent());
		if(PrimitiveComponent.IsValid())
		{
			PrimitiveComponent->SetCollisionResponseToChannel(COLLISION_LIGHT_DIRECTOR, ECR_Block);
		}
	}
}

void ALightSourceAndTargtetActor::ActivateLight()
{
	if(SpotLightComponent != nullptr)
	{
		SpotLightComponent->SetActive(true);
	}
}

void ALightSourceAndTargtetActor::DeactivateLight()
{
	if(SpotLightComponent != nullptr)
	{
		SpotLightComponent->SetActive(false);
	}
}

// Called every frame
void ALightSourceAndTargtetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(!bIsFinished)
	{
		if(bRotate)
		{
			if(TimeToRotate > CurrentRotationTime)
			{
				CurrentRotationTime += DeltaTime;
				const float TimeAlpha = FMath::Clamp(CurrentRotationTime / TimeToRotate, 0, 1);
				const float Alpha = MoveCurve.GetRichCurveConst()->Eval(TimeAlpha);
				
				if(SpotLightComponent != nullptr)
				{
					RotatingMesh->SetWorldRotation(FMath::Lerp(StartRotation, NextRotation, Alpha));
				}
			}
			else
			{
				ActivateLight();
				bRotate = false;
				CurrentRotationTime = 0.0f;
			}
		}
		else
		{
			for(auto Redirector : RedirectActors)
			{
				if(Redirector != nullptr)
				{
					Redirector->RemoveSourceLight(this);
				}
			}
			RedirectActors.Empty();
			
			if(SpotLightComponent != nullptr)
			{
				const FVector StartLocation = SpotLightComponent->GetComponentLocation();
				const FVector EndLocation = StartLocation + (SpotLightComponent->GetForwardVector() * TraceRange);
				FCollisionShape Sphere;
				Sphere.SetSphere(10);
				FCollisionQueryParams Params;
				Params.AddIgnoredActor(this);
				TArray<FHitResult> Hits;
				GetWorld()->SweepMultiByChannel(Hits, StartLocation, EndLocation, GetActorRotation().Quaternion(), COLLISION_LIGHT_DIRECTOR, Sphere, Params);
				DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Green, false, -1, 0, 10);
				for(FHitResult Hit : Hits)
				{
#if ENABLE_DRAW_DEBUG
				
					if (bDebug)
					{
						if(Hit.bBlockingHit)
						{
							DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, -1, 0, 10);
						}
					}
#endif // ENABLE_DRAW_DEBUG	
					if(Hit.bBlockingHit)
					{
						if(Hit.GetActor() == TargetTriggerVolume)
						{
							FinishedLight();
						}
						else if(ARedirectActor* Redirector = Cast<ARedirectActor>(Hit.GetActor()))
						{
							Redirector->RedirectLight(RedirectActors, TargetTriggerVolume, this,
													  SpotLightComponent, bDebug, TraceRange);
						}
						break;
					}
				}
			}
		}
	}

}

void ALightSourceAndTargtetActor::FinishedLight()
{
	bIsFinished = true;
	for(TWeakObjectPtr<ARedirectActor> RedirectActor : RedirectActors)
	{
		RedirectActor->SourceFinished(SpotLightComponent);
	}
	
}

