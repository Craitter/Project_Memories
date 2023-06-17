// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/GameOverlayWidget.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UGameOverlayWidget::ShowInteractionPrompt(float InteractionDuration, FText DisplayText)
{
	if(InteractionNotifyPrompt != nullptr)
	{
		InteractionNotifyPrompt->SetText(DisplayText);
		InteractionNotifyPrompt->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	if(InteractionDuration > 0 && InteractionTimeBar != nullptr)
	{
		InteractionTimeBar->SetPercent(0.0f);
		InteractionTimeBar->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UGameOverlayWidget::HideInteractionPrompt()
{
	if(InteractionNotifyPrompt != nullptr)
	{
		InteractionNotifyPrompt->SetText(FText::GetEmpty());
		InteractionNotifyPrompt->SetVisibility(ESlateVisibility::Hidden);
	}
	if(InteractionTimeBar != nullptr)
	{
		APlayerController* PC = GetOwningPlayer();
		if(PC != nullptr)
		{
			PC->GetWorldTimerManager().ClearTimer(ProgressbarTimerHandle);
		}
		InteractionTimeBar->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UGameOverlayWidget::StartInteractionTimer(float InteractionDuration)
{
	DesiredInteractionTime = InteractionDuration;
	APlayerController* PC = GetOwningPlayer();
	if(PC != nullptr)
	{
		PC->GetWorldTimerManager().SetTimer(ProgressbarTimerHandle, this, &ThisClass::UpdateInteractionTimer, ProgressBarTick, true);
	}
}

void UGameOverlayWidget::StopInteractionTimer()
{
	APlayerController* PC = GetOwningPlayer();
	if(PC != nullptr)
	{
		PC->GetWorldTimerManager().ClearTimer(ProgressbarTimerHandle);
	}
	if(InteractionTimeBar != nullptr)
	{
		CurrentInteractionTime = 0.0f;
		InteractionTimeBar->SetPercent(0.0f);
	}
}

void UGameOverlayWidget::UpdateInteractionTimer()
{
	CurrentInteractionTime += ProgressBarTick;
	if(InteractionTimeBar != nullptr)
	{
		InteractionTimeBar->SetPercent(CurrentInteractionTime / DesiredInteractionTime);
	}
}
