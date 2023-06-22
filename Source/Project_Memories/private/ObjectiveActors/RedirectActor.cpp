// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectiveActors/RedirectActor.h"

#include "Components/SpotLightComponent.h"
#include "Engine/TriggerVolume.h"
#include "ObjectiveActors/LightSourceAndTargtetActor.h"
#include "Project_Memories/Project_Memories.h"
#include "Subsystems/ObjectiveSubsystem.h"

// Sets default values
ARedirectActor::ARedirectActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
	
	RotatingMesh = CreateDefaultSubobject<UStaticMeshComponent>("RotatingMesh");
	if(IsValid(RotatingMesh))
	{
		SetRootComponent(RotatingMesh);
		RotatingMesh->SetCollisionResponseToChannel(COLLISION_LIGHT_DIRECTOR, ECR_Block);
		RotatingMesh->Mobility = EComponentMobility::Movable;
		RotatingMesh->SetCollisionResponseToChannel(COLLISION_INTERACTABLE, ECR_Overlap);
	}
	SpotLightComponent = CreateDefaultSubobject<USpotLightComponent>("SpotLight");
	if(IsValid(SpotLightComponent))
	{
		SpotLightComponent->SetupAttachment(RotatingMesh);
		SpotLightComponent->SetCastShadows(false);
		SpotLightComponent->SetIntensity(0.0f);
	}


}

bool ARedirectActor::HasInteractionStateChanged_Implementation(UPrimitiveComponent* InteractionComponent)
{
	if(StateChanged)
	{
		StateChanged = false;
		return true;
	}
	return false;
}

FInteractMessageInformation ARedirectActor::GetInteractionMessageType_Implementation(
	UPrimitiveComponent* InteractionComponent) const
{
	return MessageInfo;
}

bool ARedirectActor::IsAvailableForInteraction_Implementation(AActor* InteractingActor,
                                                              UPrimitiveComponent* InteractionComponent) const
{
	if(bIsFinished || bShouldRotate)
	{
		return false;
	}
	else
	{
		return ObjectiveSubsystem->IsInteractable;
	}
}

void ARedirectActor::PostInteract_Implementation(AActor* InteractingActor, UPrimitiveComponent* InteractionComponent)
{
	DeactivateLight();
	StartRotation = GetActorRotation();
	NextRotation = StartRotation;
	NextRotation.Yaw += 90.0f;
	bShouldRotate = true;
}

// Called when the game starts or when spawned
void ARedirectActor::BeginPlay()
{
	Super::BeginPlay();
	ObjectiveSubsystem = GetWorld()->GetSubsystem<UObjectiveSubsystem>();
}

// Called every frame
void ARedirectActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(!bIsFinished)
	{
		if(bShouldRotate)
		{
			if(TimeToRotate > CurrentRotationTime)
			{
				CurrentRotationTime += DeltaTime;
				const float TimeAlpha = FMath::Clamp(CurrentRotationTime / TimeToRotate, 0, 1);
				const float Alpha = MoveCurve.GetRichCurveConst()->Eval(TimeAlpha);
				SetActorRotation(FMath::Lerp(StartRotation, NextRotation, Alpha));
			}
			else
			{
				StateChanged = true;
				ActivateLight(CachedIntensity);
				bShouldRotate = false;
				CurrentRotationTime = 0.0f;
			}
		}
	}
	
}

void ARedirectActor::RedirectLight(TArray<TWeakObjectPtr<ARedirectActor>>& RedirectActors,
	TWeakObjectPtr<ATriggerVolume> TargetTriggerVolume, TWeakObjectPtr<ALightSourceAndTargtetActor> SourceActor,
	TWeakObjectPtr<USpotLightComponent> SourceSpotLight, bool bDebug, float TraceRange)
{
	UE_LOG(LogTemp, Warning , TEXT("%s %s() "), *GetName(), *FString(__FUNCTION__));
	if(!bIsFinished)
	{
		if(!bShouldRotate)
		{
			if(SpotLightComponent != nullptr)
			{
				for(auto TempRedirector : RedirectActors)
				{
					if(TempRedirector->GetName().Equals(GetName()))
					{
						return;
					}
				}
				RedirectActors.AddUnique(this);
				UpdateSpotLight(SourceSpotLight);
				UniqueLightSources.AddUnique(SourceActor);
				ActivateLight(SourceSpotLight->Intensity);
				const FVector StartLocation = SpotLightComponent->GetComponentLocation();
				const FVector EndLocation = StartLocation + (SpotLightComponent->GetForwardVector() * TraceRange);
				FCollisionShape Sphere;
				Sphere.SetSphere(10);
				FCollisionQueryParams Params;
				Params.AddIgnoredActor(this);
				TArray<FHitResult> Hits;
				GetWorld()->SweepMultiByChannel(Hits, StartLocation, EndLocation, GetActorRotation().Quaternion(), COLLISION_LIGHT_DIRECTOR, Sphere, Params);
				DrawDebugLine(GetWorld(), StartLocation, EndLocation,  FColor::Green, false, -1, 0, 10);
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
							if(SourceActor.IsValid())
							{
								SourceActor->FinishedLight();
							}
						}
						else if(ARedirectActor* Redirector = Cast<ARedirectActor>(Hit.GetActor()))
						{
							
							Redirector->RedirectLight(RedirectActors, TargetTriggerVolume, SourceActor,
													  SpotLightComponent, bDebug, TraceRange);
						}
						break;
					}
				}
			}
		}
	}
}

void ARedirectActor::SourceFinished(TWeakObjectPtr<USpotLightComponent> SourceSpotLight)
{
	bIsFinished = true;
	UpdateSpotLight(SourceSpotLight);
}

void ARedirectActor::ActivateLight(float NewIntensity) const
{
	if(SpotLightComponent != nullptr)
	{
		SpotLightComponent->SetIntensity(NewIntensity);
	}
}

void ARedirectActor::DeactivateLight()
{
	if(SpotLightComponent != nullptr)
	{
		CachedIntensity = SpotLightComponent->Intensity;
		SpotLightComponent->SetIntensity(0.0f);
	}
}

void ARedirectActor::UpdateSpotLight(TWeakObjectPtr<USpotLightComponent> InSpotLight)
{
	if(!IsValid(SpotLightComponent) || !InSpotLight.IsValid())
	{
		return;
	}
	if(LastSpotLight == InSpotLight && !bIsFinished)
	{
		return;
	}
	SpotLightComponent->Intensity = InSpotLight->Intensity;
	SpotLightComponent->LightColor = InSpotLight->LightColor;
	SpotLightComponent->AttenuationRadius = InSpotLight->AttenuationRadius;
	SpotLightComponent->InnerConeAngle = InSpotLight->InnerConeAngle;
	SpotLightComponent->OuterConeAngle = InSpotLight->OuterConeAngle;
	SpotLightComponent->SourceRadius = InSpotLight->SourceRadius;
	SpotLightComponent->SoftSourceRadius = InSpotLight->SoftSourceRadius;
	SpotLightComponent->SourceLength = InSpotLight->SourceLength;
	SpotLightComponent->bUseTemperature = InSpotLight->bUseTemperature;
	SpotLightComponent->Temperature = InSpotLight->Temperature;
	SpotLightComponent->bAffectsWorld = InSpotLight->bAffectsWorld;
	SpotLightComponent->CastShadows = InSpotLight->CastShadows;
	SpotLightComponent->VolumetricScatteringIntensity = InSpotLight->VolumetricScatteringIntensity;
	SpotLightComponent->IndirectLightingIntensity = InSpotLight->IndirectLightingIntensity;
	//now advanced would come, ill leave it
	SpotLightComponent->LightmassSettings = InSpotLight->LightmassSettings;
	SpotLightComponent->MaxDrawDistance = InSpotLight->MaxDrawDistance;
	SpotLightComponent->MaxDistanceFadeRange = InSpotLight->MaxDistanceFadeRange;
	SpotLightComponent->LightFunctionMaterial = InSpotLight->LightFunctionMaterial;
	SpotLightComponent->LightFunctionScale = InSpotLight->LightFunctionScale;
	SpotLightComponent->LightFunctionFadeDistance = InSpotLight->LightFunctionFadeDistance;
	SpotLightComponent->DisabledBrightness = InSpotLight->DisabledBrightness;
	SpotLightComponent->bUseRayTracedDistanceFieldShadows = InSpotLight->bUseRayTracedDistanceFieldShadows;
	//Again leave advanced
	LastSpotLight = InSpotLight;
}

void ARedirectActor::RemoveSourceLight(TWeakObjectPtr<ALightSourceAndTargtetActor> InSource)
{
	UniqueLightSources.Remove(InSource);
	if(UniqueLightSources.Num() <= 0 && IsValid(SpotLightComponent))
	{
		DeactivateLight();
	}
}

