// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/ObjectiveSubsystem.h"

#include "FoliageInstancedStaticMeshComponent.h"
#include "MemoriesCharacter.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Sound/SoundCue.h"

bool UObjectiveSubsystem::IsGameFInished()
{
	return bGameFinished;
}

void UObjectiveSubsystem::MeltLevelDown(float TotalMeltTime, float MeltDeltaTime)
{

	if(MemoriesCharacter.IsValid())
	{
		MeltCurve = MemoriesCharacter->MeltCurve;
	}
	DeltaTime = MeltDeltaTime;
	MeltTime = TotalMeltTime;
	ParameterCollectionInstance = GetWorld()->GetParameterCollectionInstance(ParameterCollection);
	const TObjectPtr<UMaterialInstanceDynamic> MeltMaterial = UMaterialInstanceDynamic::Create(MeltingMaterialInstance, this);
	
	UGameplayStatics::GetAllActorsOfClass(this, AActor::StaticClass(), AllActors);
	for(TWeakObjectPtr<AActor> IterateActor : AllActors)
	{
		
		if(IterateActor.IsValid())
		{
			TArray<UStaticMeshComponent*> TempMeshes;
			IterateActor->GetComponents(TempMeshes, true);
			AllStaticMeshes.Append(TempMeshes);

			TArray<UInstancedStaticMeshComponent*> TempInstancedStaticMeshComponents;
			IterateActor->GetComponents(TempInstancedStaticMeshComponents, true);
			AllInstancedStaticMeshes.Append(TempInstancedStaticMeshComponents);

			TArray<USkeletalMeshComponent*> TempSkeletals;
			IterateActor->GetComponents(TempSkeletals);
			AllSkeletals.Append(TempSkeletals);

			TArray<UTextRenderComponent*> TempTextRender;
			IterateActor->GetComponents(TempTextRender);
			AllTextRender.Append(TempTextRender);

			TArray<UFoliageInstancedStaticMeshComponent*> TempFoliage;
			IterateActor->GetComponents(TempFoliage);
			InstancedFoliageActors.Append(TempFoliage);

			// TWeakObjectPtr<ALandscape> Landscape = Cast<ALandscape>(IterateActor);
			// if(Landscape.IsValid())
			// {
			// 	Landscapes.Add(Landscape.Get());
			// }			
		}
	}
	for(TWeakObjectPtr<UStaticMeshComponent> IterateStaticMesh : AllStaticMeshes)
	{
		if(IterateStaticMesh.IsValid())
		{
			IterateStaticMesh->SetMaterial(0, MeltMaterial);
		}
	}
	for(TWeakObjectPtr<UInstancedStaticMeshComponent> IterateStaticMeshInstance : AllInstancedStaticMeshes)
	{
		if(IterateStaticMeshInstance.IsValid())
		{
			IterateStaticMeshInstance->SetMaterial(0, MeltMaterial);
		}
	}
	for(TWeakObjectPtr<USkeletalMeshComponent> IterateSkeletals : AllSkeletals)
	{
		if(IterateSkeletals.IsValid())
		{
			IterateSkeletals->SetMaterial(0, MeltMaterial);
		}
	}
	for(TWeakObjectPtr<UTextRenderComponent> IterateTextRender : AllTextRender)
	{
		if(IterateTextRender.IsValid())
		{
			IterateTextRender->SetMaterial(0, MeltMaterial);
		}
	}
	// for(TWeakObjectPtr<ALandscape> IterateTextRender : Landscapes)
	// {
	// 	if(IterateTextRender.IsValid())
	// 	{
	// 		ALandscape::Set
	// 		IterateTextRender->SetMaterial(0, MeltMaterial);
	// 	}
	// }
	for(TWeakObjectPtr<UFoliageInstancedStaticMeshComponent> IterateTextRender : InstancedFoliageActors)
	{
		if(IterateTextRender.IsValid())
		{
			IterateTextRender->SetMaterial(0, MeltMaterial);
		}
	}
	GetWorld()->GetTimerManager().SetTimer(MeltHandle, this, &UObjectiveSubsystem::UpdateMelting, DeltaTime, true);
}

UObjectiveSubsystem::UObjectiveSubsystem()
{
	static ConstructorHelpers::FObjectFinder<USoundCue> ClockBPSound(TEXT("/Game/SFX/Clock"));
	ClockFinishSound = ClockBPSound.Object;
	
	static ConstructorHelpers::FObjectFinder<USoundCue> ObjectiveBPSound(TEXT("/Game/SFX/Stinger"));
	OneObjectiveFinishedSound = ObjectiveBPSound.Object;

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> MaterialInstance(TEXT("/Game/SpecialMaterial/M_Instance_SpecialDissolve"));
	MeltingMaterialInstance = MaterialInstance.Object;

	static ConstructorHelpers::FObjectFinder<UMaterialParameterCollection> ParameterCollections(TEXT("/Game/SpecialMaterial/SpecialCollection"));
	ParameterCollection = ParameterCollections.Object;
	
}

void UObjectiveSubsystem::TrackClock(TWeakObjectPtr<AObjectiveClock> InClock, bool bIsLeading)
{
	if(bIsLeading)
	{
		LeadingClock = InClock;
	}
	else
	{
		ListeningClocks.AddUnique(InClock);
	}
	bClocksFinished = false;
}

void UObjectiveSubsystem::SetClockSyncRequestDelegate(FSyncRequestDelegate& SyncDelegate)
{
	SyncDelegate.BindUObject(this, &UObjectiveSubsystem::OnSyncRequest);
}

void UObjectiveSubsystem::OnSyncRequest(FRotator TargetRotation, float Tolerance)
{
	
	bool bAllInSync = true;
	for(TWeakObjectPtr<AObjectiveClock> InClock : ListeningClocks)
	{
		if(InClock.IsValid())
		{
			if(InClock->GetHourHandRotation().Equals(TargetRotation, Tolerance) && !InClock->IsStopped())
			{
				
				if(!InClock->Locked)
				{
					InClock->SetClockLocked();
					UGameplayStatics::PlaySound2D(this, ClockFinishSound);
				}
			}
			else
			{
				bAllInSync = false;
			}
		}
	}
	if(bAllInSync)
	{
		
		for(TWeakObjectPtr<AObjectiveClock> InClock : ListeningClocks)
		{
			if(InClock.IsValid())
			{
				InClock->StartFinishingClock();
			}
		}
		if(LeadingClock.IsValid())
		{
			LeadingClock->StartFinishingClock();
		}
		if(MemoriesCharacter.IsValid())
		{
			MemoriesCharacter->StopClockTicking();
		}
		bClocksFinished = true;
		TryFinishGame();
	}
}

void UObjectiveSubsystem::SetPlayerCharacter(TWeakObjectPtr<AMemoriesCharacter> InMemoriesCharacter)
{
	MemoriesCharacter = InMemoriesCharacter;	
}

void UObjectiveSubsystem::TryFinishGame()
{
	GetWorld()->GetTimerManager().SetTimer(ObjectiveFinishedHandle, this, &UObjectiveSubsystem::ObjectiveFinished, 2.1f);
	if(bClocksFinished && bSpotlightsFinished && bMovableFinished && bFireBowlsFinished)
	{
		UE_LOG(LogTemp, Warning , TEXT("%s() GameFInished"), *FString(__FUNCTION__));
		bGameFinished = true;
	}
}

void UObjectiveSubsystem::ObjectiveFinished()
{
	UGameplayStatics::PlaySound2D(this, OneObjectiveFinishedSound);
}

void UObjectiveSubsystem::TrackSpotlight(TWeakObjectPtr<ALightSourceAndTargtetActor> Light)
{
	Spotlights.AddUnique(Light);
	bSpotlightsFinished = false;
	
}

void UObjectiveSubsystem::FinishSpotLight()
{
	SpotlightIndex++;
	UGameplayStatics::PlaySound2D(this, ClockFinishSound);
	if(SpotlightIndex == Spotlights.Num())
	{
		bSpotlightsFinished = true;
		TryFinishGame();
	}
}

void UObjectiveSubsystem::TrackMovable(TWeakObjectPtr<AMovableActor> MovableActor)
{
	Movables.AddUnique(MovableActor);
	bMovableFinished = false;
}

void UObjectiveSubsystem::FinishMovable()
{
	MovableIndex++;
	UGameplayStatics::PlaySound2D(this, ClockFinishSound);
	if(Movables.Num() == MovableIndex)
	{
		bMovableFinished = true;
		TryFinishGame();
	}
}

void UObjectiveSubsystem::TrackBasin(TWeakObjectPtr<ATorchBasin> Basin)
{
	Basins.AddUnique(Basin);
	bFireBowlsFinished = false;
}

void UObjectiveSubsystem::FinishBasin()
{
	BasinIndex++;
	UGameplayStatics::PlaySound2D(this, ClockFinishSound);
	if(BasinIndex == Basins.Num())
	{
		bFireBowlsFinished = true;
		TryFinishGame();
	}
}

bool UObjectiveSubsystem::CanPassClocks()
{
	if(bClocksFinished && bMovableFinished && bFireBowlsFinished)
	{
		return true;
	}
	return false;
}

void UObjectiveSubsystem::OnMeltAway()
{
	UE_LOG(LogTemp, Warning , TEXT("Melt Finished"));
	GetWorld()->GetTimerManager().ClearTimer(MeltHandle);
}

void UObjectiveSubsystem::UpdateMelting()
{
	if(MeltTime > CurrentMeltTime)
	{
		CurrentMeltTime += DeltaTime;
		if(ParameterCollectionInstance != nullptr)
		{
			const float TimeAlpha = FMath::Clamp(CurrentMeltTime / MeltTime, 0, 1);
			const float Alpha = MeltCurve.GetRichCurveConst()->Eval(TimeAlpha);
			const float NewValue = FMath::Lerp(-0.3f, 1.0f, Alpha);
			ParameterCollectionInstance->SetScalarParameterValue(TEXT("Dissolve"), NewValue);
		}
	}
	else
	{
		
		OnMeltAway();
	}
}

