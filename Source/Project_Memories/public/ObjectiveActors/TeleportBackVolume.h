// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerVolume.h"
#include "TeleportBackVolume.generated.h"

class UBoxComponent;
class UObjectiveSubsystem;
/**
 * 
 */
UCLASS()
class PROJECT_MEMORIES_API ATeleportBackVolume : public AActor
{
	GENERATED_BODY()

public:

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UBoxComponent> TriggerBox = {nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector PositionToGoBackTo;
private:
};
