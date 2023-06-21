// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectiveActors/FastSwappingTextActor.h"

#include "Components/TextRenderComponent.h"

void AFastSwappingTextActor::BeginPlay()
{
	Super::BeginPlay();

	CurrentIndex = -1;
	ShowNextText();
	GetWorldTimerManager().SetTimer(NextText, this, &ThisClass::ShowNextText, TextSwapSpeed, true);
}

AFastSwappingTextActor::AFastSwappingTextActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AFastSwappingTextActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AFastSwappingTextActor::ShowNextText()
{
	if(UTextRenderComponent* TextRenderComponent = GetTextRender())
	{
		CurrentIndex++;
		if(!TextsToShow.IsValidIndex(CurrentIndex))
		{
			CurrentIndex = 0;
		}		
		TextRenderComponent->SetText(TextsToShow[CurrentIndex].Text);
	}
}

void AFastSwappingTextActor::ClearText()
{
	GetWorldTimerManager().ClearTimer(NextText);
	if(UTextRenderComponent* TextRenderComponent = GetTextRender())
	{
		TextRenderComponent->SetVisibility(false);
	}
}
