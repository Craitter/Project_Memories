// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableInterface.h"
#include "NiagaraActions.h"
#include "GameFramework/Character.h"
#include "MemoriesCharacter.generated.h"

class USoundCue;
class AFastSwappingTextActor;
class UInteractionComponent;
enum class EInteractMessageType : uint8;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class PROJECT_MEMORIES_API AMemoriesCharacter : public ACharacter, public IInteractableInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMemoriesCharacter();
	
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	UPROPERTY(BlueprintReadWrite)
	bool bShouldRotateToInteractable = false;

	UPROPERTY(BlueprintReadWrite)
	FVector DialogueFacingTargetLocation = FVector::ZeroVector;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Essential Components")
	TObjectPtr<USpringArmComponent> SpringArmComponent = {nullptr};

	UPROPERTY(VisibleAnywhere, Category = "Essential Components")
	TObjectPtr<UCameraComponent> CameraComponent = {nullptr};

	UPROPERTY(VisibleAnywhere, Category = "Essential Components")
	TObjectPtr<UInteractionComponent> InteractionComponent = {nullptr};

	UPROPERTY(VisibleAnywhere, Category = "Essential Components")
	TObjectPtr<UStaticMeshComponent> Torch = {nullptr};
	UPROPERTY(VisibleAnywhere, Category = "Essential Components")
	TObjectPtr<UStaticMeshComponent> TorchDecoration = {nullptr};

	UPROPERTY(VisibleAnywhere, Category = "Essential Components")
	TObjectPtr<UNiagaraComponent> NiagaraComponent = {nullptr};
	//This limits the pitch rotation minimum(direction: ground) of the Camera. Default Values are "stolen"
	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	float ViewPitchMin = -80.0f;
	//This limits the pitch rotation minimum(direction: air) of the Camera. Default Values are "stolen"
	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	float ViewPitchMax = 55.0f;
	
	//See CharacterMovement: Walking : MaxWalkSpeed for the normal speed and how it behaves
	UPROPERTY(Category="Character Movement: Running", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0", ForceUnits="cm/s"))
	float MaxRunSpeed = 900.f;

	//the time in seconds it takes to rotate to the Person talking to
	UPROPERTY(EditDefaultsOnly)
	float DialogueRotationTime = 1.0f;

	UPROPERTY(BlueprintReadWrite)
	bool bShouldRotateToPlayerCharacter = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UAudioComponent> ClockTicking;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAudioComponent> FootStep;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable)
	void PauseClockTicking();
	UFUNCTION(BlueprintCallable)
	void StartClockTIcking();

	float VolumeTIcking;
	virtual void Jump() override;
	//Begin InputCallbacks
	void TryStartRun();
	void TryStopRun();
	bool IsRunning() const;
	void TryCrouch();
	void TryBeginInteract();
	void StopInteract();
	//End InputCallbacks
	void GetCameraPitchMinMax(float& Min, float& Max);
	FVector GetCameraForwardVector();
	
	UFUNCTION(BlueprintCallable)
	void ResetInteractionTrace();

	UFUNCTION(BlueprintCallable)
	void ClearFastText();

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAudioComponent> PlayingSound = {nullptr};

	UPROPERTY(BlueprintReadWrite)
	bool bPlaySadAnimation = false;
	
	UPROPERTY(BlueprintReadWrite)
	bool bIsInteractable = true;

	void StopClockTicking();
	
	UFUNCTION(BlueprintCallable)
	void StopMusic();
	
	UFUNCTION(BlueprintCallable)
	void EnableTorch();

	bool IsTorchEnabled() const;
	//Begin InteractCOmponentCalls
	//End InteractComponentCalls

	UPROPERTY(EditInstanceOnly)
	TObjectPtr<AFastSwappingTextActor> TextToVanishAfterDialogue = {nullptr};
private:
	float InternMaxWalkSpeed = 0.0f;
	float CurrentRotationTime = 0.0f;

	bool bIsRunning = false;
	bool bTriesToStartRunning = false;

	bool bHasTorch = false;
};
