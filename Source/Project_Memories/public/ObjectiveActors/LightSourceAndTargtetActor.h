// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableInterface.h"
#include "GameFramework/Actor.h"
#include "LightSourceAndTargtetActor.generated.h"

class ARedirectActor;
class ATriggerVolume;
class USpotLightComponent;


UCLASS()
class PROJECT_MEMORIES_API ALightSourceAndTargtetActor : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALightSourceAndTargtetActor();

	virtual bool IsAvailableForInteraction_Implementation(AActor* InteractingActor, UPrimitiveComponent* InteractionComponent) const override;

	virtual void PostInteract_Implementation(AActor* InteractingActor, UPrimitiveComponent* InteractionComponent) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	
	UPROPERTY(VisibleAnywhere, Category = "LightSource")
	TObjectPtr<UStaticMeshComponent> RotatingMesh = {nullptr};

	UPROPERTY(VisibleAnywhere, Category = "LightSource")
	TObjectPtr<USpotLightComponent> SpotLightComponent = {nullptr};

	UPROPERTY(EditInstanceOnly, Category = "LightSource")
	TObjectPtr<ATriggerVolume> TargetTriggerVolume = {nullptr};

	UPROPERTY(EditAnywhere, Category = "LightSource")
	float TimeToRotate = 0.5f;

	UPROPERTY(EditInstanceOnly, Category = "LightSource")
	bool bDebug = false;

	//this will depend on how far the light goes,since attenuation Radius is not a simple float distance we have to set the Range of the light to impact triggers and Redirectors manually THIS WILL NOT CHANGE THE VISUAL APPEREANCE OF THE LIGHT
	UPROPERTY(EditAnywhere, Category = "LightSource")
	float TraceRange = 1000.0f;
	
	UPROPERTY(EditAnywhere, Category = "LightSource")
	FRuntimeFloatCurve MoveCurve;
private:
	bool bIsFinished = false;
	bool bRotate = false;
	float CurrentRotationTime = 0.0f;
	FRotator NextRotation = FRotator::ZeroRotator;
	FRotator StartRotation = FRotator::ZeroRotator;

	TArray<TWeakObjectPtr<ARedirectActor>> RedirectActors;

	void ActivateLight();
	void DeactivateLight();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void FinishedLight();
};
