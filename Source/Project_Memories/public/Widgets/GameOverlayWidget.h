// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameOverlayWidget.generated.h"

class UTextBlock;
class UProgressBar;
class UMenuAnchor;
/**
 * 
 */
UCLASS()
class PROJECT_MEMORIES_API UGameOverlayWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void ShowInteractionPrompt(float InteractionDuration, FText DisplayText);
	void HideInteractionPrompt();
	void StartInteractionTimer(float InteractionDuration);
	void StopInteractionTimer();
	
private:
	void UpdateInteractionTimer();
	//in sec
	UPROPERTY(EditDefaultsOnly)
	float ProgressBarTick = 0.05f;
	float CurrentInteractionTime = 0.0f;
	float DesiredInteractionTime = 0.0f;
	FTimerHandle ProgressbarTimerHandle;
	
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> InteractionNotifyPrompt = {nullptr};
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> InteractionTimeBar = {nullptr};
	
};
