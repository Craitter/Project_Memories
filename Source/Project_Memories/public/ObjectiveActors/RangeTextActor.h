// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TextRenderActor.h"
#include "RangeTextActor.generated.h"

class UObjectiveSubsystem;
class USphereComponent;
/**
 * 
 */
UCLASS()
class PROJECT_MEMORIES_API ARangeTextActor : public ATextRenderActor
{
	GENERATED_BODY()

private:
	ARangeTextActor();

public:
	virtual void Tick(float DeltaSeconds) override;


	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
protected:
	virtual void BeginPlay() override;

	
	TObjectPtr<USphereComponent> CollisionSphere = {nullptr};

	UPROPERTY(EditAnywhere)
	bool bTextVanishesWhenLeavingDistance = false;

	UPROPERTY(EditAnywhere)
	float TextShowHideRange = 100.0f;

	TWeakObjectPtr<UObjectiveSubsystem> ObjectiveSubsystem = nullptr;
};
