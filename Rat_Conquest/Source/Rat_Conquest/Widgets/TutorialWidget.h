// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TutorialWidget.generated.h"

/**
 * 
 */
UCLASS()
class RAT_CONQUEST_API UTutorialWidget : public UUserWidget
{
	GENERATED_BODY()

public: 

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Tutorial Widget")
	class UTextBlock* TutorialText;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Tutorial Widget")
	class UButton* NextButton;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Tutorial Widget")
	class UButton* PreviousButton;

	virtual void NativeConstruct() override;

	void SetText(FText text);

	UFUNCTION()
	void NextbuttonPressed();
	UFUNCTION()
	void PreviousButtonPressed();

	UPROPERTY()
	class ATutorialManager* TutorialManager;
};
