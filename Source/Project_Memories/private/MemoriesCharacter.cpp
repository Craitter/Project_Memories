// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Memories/public/MemoriesCharacter.h"

// Sets default values
AMemoriesCharacter::AMemoriesCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

}

// Called to bind functionality to input
void AMemoriesCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

