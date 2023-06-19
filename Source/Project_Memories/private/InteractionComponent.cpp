// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionComponent.h"

#include "InteractableInterface.h"
#include "MemoriesCharacter.h"
#include "Project_Memories/Project_Memories.h"

// Sets default values for this component's properties
UInteractionComponent::UInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	OwningCharacter = Cast<AMemoriesCharacter>(GetOwner());
	if(OwningCharacter.IsValid() && OwningCharacter->IsPlayerControlled())
	{
		OwningCharacter->GetWorldTimerManager().SetTimer(InteractionTraceTimerHandle, this, &ThisClass::PerformTrace, InteractionLoopTime, true);
		OnInteractableFound.BindUObject(this, &ThisClass::InteractableFound);
	}
	
	
	// ...
	
}


void UInteractionComponent::InteractableFound(FHitResult InteractableHit)
{
	if(!InteractableHit.bBlockingHit)
	{
		if(CachedHit.bBlockingHit)
		{
			if(OwningCharacter.IsValid() && OnInteractableLost.IsBound())
			{
				bHasInteractableTarget = false;
				if(bIsInteracting)
				{
					StopInteract();
				}
				OnInteractableLost.Execute();
			}
		}
		CachedHit = InteractableHit;
	}
	else
	{
		const IInteractableInterface* III = Cast<IInteractableInterface>(InteractableHit.GetActor());

		
		
		bool BroadcastNewTarget = true;
		if(CachedHit.GetActor() == InteractableHit.GetActor() && CachedHit.GetComponent() == InteractableHit.GetComponent() &&
			III && !III->Execute_HasInteractionStateChanged(InteractableHit.GetActor(), InteractableHit.GetComponent()))
		{
			BroadcastNewTarget = false;
			
		}
		else if(CachedHit.GetActor() != nullptr)
		{
			bHasInteractableTarget = false;
			if(bIsInteracting)
			{
				StopInteract();
			}
			OnInteractableLost.Execute();
		}
		if(BroadcastNewTarget)
		{
			if(III == nullptr)
			{
				return;
			}
			
			const FInteractMessageInformation MessageInformation = III->Execute_GetInteractionMessageType(InteractableHit.GetActor(), InteractableHit.GetComponent());
			const float InteractDuration = III->Execute_GetInteractionDuration(InteractableHit.GetActor(), InteractableHit.GetComponent());
			
			bHasInteractableTarget = true;
			OnNewInteractableFound.Execute(InteractDuration, MessageInformation);
			CachedHit = InteractableHit;
		}
	}
}

// Called every frame
void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UInteractionComponent::PerformTrace()
{
	TWeakObjectPtr<UWorld> World = GetWorld();
	if(!World.IsValid() || !OwningCharacter.IsValid() || CollisionProfileName.Name == NAME_None)
	{
		UE_LOG(LogTemp, Error , TEXT("%s() InteractionTrace failed "), *FString(__FUNCTION__));
		return;
	}
	FVector StartLocation = OwningCharacter->GetPawnViewLocation();
	FVector Direction = OwningCharacter->GetCameraForwardVector();
	FVector EndLocation = StartLocation + Direction * InteractionRange;
	FName ProfileName = CollisionProfileName.Name;
	TWeakObjectPtr<AActor> Owner = GetOwner();
	FCollisionQueryParams Params;
	Params.bReturnPhysicalMaterial = true;
	Params.AddIgnoredActor(GetOwner());
	
	bool bDebugOut = bDebug;
	float LoopTime = InteractionLoopTime;
	float SphereRadius = InteractionSphereRadius;
	
	FOnInteractableFound* OutDelegate = &OnInteractableFound;
	
	// AsyncTask(ENamedThreads::AnyHiPriThreadNormalTask,
	// 	[World, StartLocation, EndLocation, ProfileName, Params, bDebugOut, LoopTime, Owner, OutDelegate, SphereRadius]
	// {
		TArray<FHitResult> HitResults;
		World->LineTraceMultiByProfile(HitResults, StartLocation, EndLocation, ProfileName, Params);
		
		FVector InEndLocation = FVector::ZeroVector;
		for(const FHitResult& Hit : HitResults)
		{
			if(Hit.GetActor() != nullptr && Hit.GetActor() != Owner)
			{
				InEndLocation = Hit.Location;
			}
		}
		
		if(InEndLocation == FVector::ZeroVector)
		{
			InEndLocation = EndLocation;
		}
		HitResults.Empty();
		FHitResult OutResult;
		OutResult.bBlockingHit = false;
		bool bFoundInteractable = false;
		World->LineTraceMultiByProfile(HitResults, StartLocation, EndLocation, ProfileName, Params);
		for(const FHitResult& Hit : HitResults)
		{
			if(Hit.GetActor() != nullptr && Hit.GetActor() != Owner)
			{
				if(Hit.Component.IsValid() && Hit.Component.Get()->GetCollisionResponseToChannel(COLLISION_INTERACTABLE) == ECR_Overlap)
				{
					const bool bIsInteractable = Hit.GetActor()->Implements<UInteractableInterface>();

					if(bIsInteractable && IInteractableInterface::Execute_IsAvailableForInteraction(Hit.GetActor(), GetOwner(), Hit.Component.Get()))
					{
						//if no response we can cast
						OutResult = Hit;
						OutResult.bBlockingHit = true;
						bFoundInteractable = true;
						break;
					}
				}
			}
			if(Hit.bBlockingHit)
			{
				OutResult = Hit; //Todo: Make this work cause there will be no blocking hit
				OutResult.bBlockingHit = false;
			}
		}

#if ENABLE_DRAW_DEBUG
		if (bDebugOut)
		{
						
			if (OutResult.bBlockingHit)
			{
				DrawDebugLine(World.Get(), StartLocation, OutResult.Location, FColor::Red, false, LoopTime);
			}
			else
			{
				DrawDebugLine(World.Get(), StartLocation, InEndLocation, FColor::Green, false, LoopTime);
			}
		}
#endif // ENABLE_DRAW_DEBUG	
		if(!bFoundInteractable)
		{
			if(OutResult.bBlockingHit)
			{
				InEndLocation = OutResult.Location;
			}

			FCollisionShape Sphere;
			Sphere.SetSphere(SphereRadius);

			HitResults.Empty();
			World->SweepMultiByProfile(HitResults, InEndLocation, InEndLocation, FQuat::Identity, ProfileName, Sphere, Params);
			
			for(const FHitResult& Hit : HitResults)
			{
				if(Hit.GetActor() != nullptr && Hit.GetActor() != Owner)
				{
					if(Hit.Component.IsValid() && Hit.Component.Get()->GetCollisionResponseToChannel(COLLISION_INTERACTABLE) == ECR_Overlap)
					{
						const bool bIsInteractable = Hit.GetActor()->Implements<UInteractableInterface>();

						if(bIsInteractable && IInteractableInterface::Execute_IsAvailableForInteraction(Hit.GetActor(), GetOwner(), Hit.Component.Get()))
						{
							//if no response we can cast
							OutResult = Hit;
							OutResult.bBlockingHit = true;
							bFoundInteractable = true;
							break;
						}
					}
				}
				if(Hit.bBlockingHit)
				{
					OutResult = Hit; //Todo: Make this work cause there will be no blocking hit
					OutResult.bBlockingHit = false;
				}
			}
#if ENABLE_DRAW_DEBUG
			if (bDebugOut)
			{
					
				if (OutResult.bBlockingHit)
				{
					DrawDebugSphere(World.Get(), OutResult.Location, SphereRadius, 16, FColor::Red, false, LoopTime);
				}
				else
				{
					DrawDebugSphere(World.Get(), InEndLocation, SphereRadius, 16, FColor::Green, false, LoopTime);
				}
			}
#endif // ENABLE_DRAW_DEBUG
		}
			OutResult.bBlockingHit = bFoundInteractable;

	// AsyncTask(ENamedThreads::GameThread, [OutDelegate, OutResult] ()
	// 	{
			// We execute the delegate along with the param
		if(OutDelegate != nullptr && OutDelegate->IsBound())
		{
			OutDelegate->Execute(OutResult);
		} 
		// });
	// });
}

void UInteractionComponent::StartInteract()
{
	if(!bHasInteractableTarget || bIsInteracting)
	{
		return;
	}
	bIsInteracting = true;
	IInteractableInterface* III = Cast<IInteractableInterface>(CachedHit.GetActor());
	if(III == nullptr)
	{
		UE_LOG(LogTemp, Error , TEXT("%s() Interactable is null but the system thinks it has one"), *FString(__FUNCTION__));
		bIsInteracting = false;
		return;
	}
	if(!III->Execute_IsAvailableForInteraction(CachedHit.GetActor(), GetOwner(), CachedHit.GetComponent()))
	{
		bIsInteracting = false;
		return;
	}
	III->Execute_PreInteract(CachedHit.GetActor(), GetOwner(), CachedHit.GetComponent());
	if(OwningCharacter.IsValid())
	{
		OwningCharacter->bShouldRotateToInteractable = true;
		OwningCharacter->DialogueFacingTargetLocation = CachedHit.GetActor()->GetActorLocation();
	}
	const float InteractDuration = III->Execute_GetInteractionDuration(CachedHit.GetActor(), CachedHit.GetComponent());
	if(InteractDuration > 0)
	{
		if(OwningCharacter.IsValid() && OnInteractionStarted.IsBound())
		{
			OwningCharacter->GetWorldTimerManager().SetTimer(InteractDurationTimerHandle, this, &ThisClass::FinishInteract, InteractDuration, false);
			OnInteractionStarted.Execute(InteractDuration);
		}
		else
		{
			UE_LOG(LogTemp, Error , TEXT("%s() OnInteractionStarted is not Bound"), *FString(__FUNCTION__));
		}
	}
	else
	{
		FinishInteract();
	}
}

void UInteractionComponent::StopInteract()
{
	if(!bIsInteracting)
	{
		return;
	}
	const IInteractableInterface* III = Cast<IInteractableInterface>(CachedHit.GetActor());
	if(III == nullptr && OnInteractableLost.IsBound())
	{
		UE_LOG(LogTemp, Error , TEXT("%s() Interactable is null but the system thinks it has one"), *FString(__FUNCTION__));
		bIsInteracting = false;
		return;
	}
	CachedHit.Reset();
	OnInteractableLost.Execute();
	if(OwningCharacter.IsValid() && OnInteractionStarted.IsBound())
	{
		OwningCharacter->GetWorldTimerManager().ClearTimer(InteractDurationTimerHandle);
	}
	bIsInteracting = false;
	
}

void UInteractionComponent::ResetInteraction()
{
	CachedHit.Reset();
}


void UInteractionComponent::FinishInteract()
{
	IInteractableInterface* III = Cast<IInteractableInterface>(CachedHit.GetActor());
	if(III == nullptr)
	{
		UE_LOG(LogTemp, Error , TEXT("%s() Interactable is null but the system thinks it has one"), *FString(__FUNCTION__));
		bIsInteracting = false;
		return;
	}
	III->Execute_PostInteract(CachedHit.GetActor(), GetOwner(), CachedHit.GetComponent());
	if(OnInteractionFinished.IsBound())
	{
		OnInteractionFinished.Execute();
	}
	if(OnInteractableLost.IsBound())
	{
		OnInteractableLost.Execute();
	}
	
	
	UE_LOG(LogTemp, Warning , TEXT("%s() InteractionFInished"), *FString(__FUNCTION__));
}
