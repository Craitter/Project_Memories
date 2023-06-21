// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TextRenderActor.h"
#include "FastSwappingTextActor.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FTextSwap
{
	GENERATED_BODY()

	/** Text content, can be multi line using <br> as line separator */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Text, meta=(MultiLine=true))
	FText Text;
};

UCLASS()
class PROJECT_MEMORIES_API AFastSwappingTextActor : public ATextRenderActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = "TextsToShow")
	TArray<FTextSwap> TextsToShow;

	UPROPERTY(EditAnywhere, Category = "TextsToShow")
	float TextSwapSpeed = 0.5f;

	virtual void BeginPlay() override;
public:
	AFastSwappingTextActor();
	
	virtual void Tick(float DeltaSeconds) override;

	FTimerHandle NextText;

	int32 CurrentIndex = 0.0f;

	void ShowNextText();

	void ClearText();
};
