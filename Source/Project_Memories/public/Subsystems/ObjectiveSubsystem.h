// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ObjectiveSubsystem.generated.h"

/**
 * 
 */

UENUM()
enum class EObjectiveType
{
	OT_SpotLight,

	OT_FireBowl,

	OT_Clock,

	
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnObjectiveCompletedDelegate, TWeakObjectPtr<AActor>, ObjectiveActor, EObjectiveType, Type);
UCLASS()
class PROJECT_MEMORIES_API UObjectiveSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
};
