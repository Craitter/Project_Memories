// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectiveActors/TorchBasin.h"

#include "MemoriesCharacter.h"
#include "NiagaraComponent.h"
#include "Project_Memories/Project_Memories.h"

// Sets default values
ATorchBasin::ATorchBasin()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("TorchBasin");
	if(IsValid(Mesh))
	{
		SetRootComponent(Mesh);
		Mesh->SetCollisionResponseToChannel(COLLISION_INTERACTABLE, ECR_Overlap);
	}
	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("NiagaraComponent");
	if(IsValid(NiagaraComponent))
	{
		NiagaraComponent->SetupAttachment(Mesh);
	}
}

bool ATorchBasin::IsAvailableForInteraction_Implementation(AActor* InteractingActor,
	UPrimitiveComponent* InteractionComponent) const
{
	const TWeakObjectPtr<AMemoriesCharacter> Char = Cast<AMemoriesCharacter>(InteractingActor);
	if(Char.IsValid() && !bIsIgnited && Char->IsTorchEnabled())
	{
		return true;
	}
	else
	{
		return false;
	}
}

FInteractMessageInformation ATorchBasin::GetInteractionMessageType_Implementation(
	UPrimitiveComponent* InteractionComponent) const
{
	return IgniteMessageInfo;
}

void ATorchBasin::PostInteract_Implementation(AActor* InteractingActor, UPrimitiveComponent* InteractionComponent)
{
	bIsIgnited = true;
	if(IsValid(NiagaraComponent))
	{
		NiagaraComponent->Activate();
	}
}

// Called when the game starts or when spawned
void ATorchBasin::BeginPlay()
{
	Super::BeginPlay();
	if(IsValid(NiagaraComponent))
	{
		NiagaraComponent->Deactivate();
	}
	
}

// Called every frame
void ATorchBasin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

