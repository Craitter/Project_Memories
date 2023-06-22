// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableInterface.h"
#include "GameFramework/Actor.h"
#include "TorchBasin.generated.h"


class ALevelSequenceActor;
class UNiagaraComponent;
class UObjectiveSubsystem;
UCLASS()
class PROJECT_MEMORIES_API ATorchBasin : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATorchBasin();

	virtual bool IsAvailableForInteraction_Implementation(AActor* InteractingActor, UPrimitiveComponent* InteractionComponent) const override;

	virtual FInteractMessageInformation GetInteractionMessageType_Implementation(UPrimitiveComponent* InteractionComponent) const override;

	virtual void PostInteract_Implementation(AActor* InteractingActor, UPrimitiveComponent* InteractionComponent) override;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Mesh = {nullptr};
	
	UPROPERTY(VisibleAnywhere, Category = "Essential Components")
	TObjectPtr<UNiagaraComponent> NiagaraComponent = {nullptr};

	UPROPERTY(EditInstanceOnly)
	TObjectPtr<ALevelSequenceActor> SequenceActor;
	
	UPROPERTY(EditAnywhere)
	FInteractMessageInformation IgniteMessageInfo;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnSequenceFinished();
private:
	bool bIsIgnited = false;
	TWeakObjectPtr<UObjectiveSubsystem> ObjectiveSubsystem = {nullptr};
};
