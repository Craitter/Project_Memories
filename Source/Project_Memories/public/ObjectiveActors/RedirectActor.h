// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableInterface.h"
#include "GameFramework/Actor.h"
#include "RedirectActor.generated.h"

class UObjectiveSubsystem;
class USpotLightComponent;
class ALightSourceAndTargtetActor;
class ATriggerVolume;

UCLASS()
class PROJECT_MEMORIES_API ARedirectActor : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARedirectActor();

	
	virtual bool HasInteractionStateChanged_Implementation(UPrimitiveComponent* InteractionComponent) override;
	virtual FInteractMessageInformation GetInteractionMessageType_Implementation(UPrimitiveComponent* InteractionComponent) const override;
	virtual bool IsAvailableForInteraction_Implementation(AActor* InteractingActor, UPrimitiveComponent* InteractionComponent) const override;
	virtual void PostInteract_Implementation(AActor* InteractingActor, UPrimitiveComponent* InteractionComponent) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Rotation")
	float TimeToRotate = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Rotation")
	FRuntimeFloatCurve MoveCurve;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void RedirectLight(TArray<TWeakObjectPtr<ARedirectActor>>& RedirectActors, TWeakObjectPtr<ATriggerVolume> TargetTriggerVolume,
	                   TWeakObjectPtr<ALightSourceAndTargtetActor> SourceActor, TWeakObjectPtr<USpotLightComponent> SourceSpotLight, bool
	                   bDebug, float TraceRange);
	void SourceFinished(TWeakObjectPtr<USpotLightComponent> SourceSpotLight);

	TWeakObjectPtr<USpotLightComponent> LastSpotLight = {nullptr};

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> RotatingMesh = {nullptr};
	

	//This Component will be updated by the SourceLight, no need to set values here 
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpotLightComponent> SpotLightComponent = {nullptr};

	UPROPERTY(EditAnywhere)
	FInteractMessageInformation MessageInfo;

	void RemoveSourceLight(TWeakObjectPtr<ALightSourceAndTargtetActor> InSource);
private:

	void ActivateLight(float NewIntensity) const;
	void DeactivateLight();
	void UpdateSpotLight(TWeakObjectPtr<USpotLightComponent> InSpotLight);
	
	bool StateChanged = false;
	float CachedIntensity = 0.0f;
	float CurrentRotationTime = 0.0f;
	bool bIsFinished = false;
	bool bShouldRotate = false;
	FRotator StartRotation = FRotator::ZeroRotator;
	FRotator NextRotation = FRotator::ZeroRotator;

	TArray<TWeakObjectPtr<ALightSourceAndTargtetActor>> UniqueLightSources;

	TWeakObjectPtr<UObjectiveSubsystem> ObjectiveSubsystem = {nullptr};
	
};
