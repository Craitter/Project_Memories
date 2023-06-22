// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectiveActors/RangeTextActor.h"

#include "MemoriesCharacter.h"
#include "Components/BillboardComponent.h"
#include "Components/SphereComponent.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/ObjectiveSubsystem.h"

ARangeTextActor::ARangeTextActor()
{
	PrimaryActorTick.bCanEverTick = true;


	CollisionSphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	if(IsValid(CollisionSphere))
	{
		CollisionSphere->SetupAttachment(RootComponent);
	}
}

void ARangeTextActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	TWeakObjectPtr<APlayerController> Controller = UGameplayStatics::GetPlayerController(this, 0);
	if(Controller.IsValid())
	{
		FRotator NewRotation = Controller->GetControlRotation();
		NewRotation.Yaw -= 180.0f;
		NewRotation.Pitch = 0.0f;
		SetActorRotation(NewRotation);
	}
}

void ARangeTextActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	UTextRenderComponent* TextRenderComponent = GetTextRender();
	const TWeakObjectPtr<AMemoriesCharacter> Char = Cast<AMemoriesCharacter>(OtherActor);
	if(Char.IsValid() && TextRenderComponent != nullptr && ObjectiveSubsystem->IsInteractable)
	{
		TextRenderComponent->SetVisibility(true);
	}
}

void ARangeTextActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(!bTextVanishesWhenLeavingDistance)
	{
		return;
	}
	UTextRenderComponent* TextRenderComponent = GetTextRender();
	const TWeakObjectPtr<AMemoriesCharacter> Char = Cast<AMemoriesCharacter>(OtherActor);
	if(Char.IsValid() && TextRenderComponent != nullptr)
	{
		TextRenderComponent->SetVisibility(false);
	}
}

void ARangeTextActor::BeginPlay()
{
	Super::BeginPlay();

	if(IsValid(CollisionSphere))
	{
		CollisionSphere->SetSphereRadius(TextShowHideRange);
		CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnOverlapBegin);
		CollisionSphere->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnOverlapEnd);
	}
	if(UTextRenderComponent* TextRenderComponent = GetTextRender())
	{
		TextRenderComponent->SetVisibility(false);
	}

	ObjectiveSubsystem = GetWorld()->GetSubsystem<UObjectiveSubsystem>();
}
