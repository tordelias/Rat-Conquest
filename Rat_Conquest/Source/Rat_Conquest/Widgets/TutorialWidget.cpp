// Fill out your copyright notice in the Description page of Project Settings.


#include "TutorialWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Rat_Conquest/Managers/TutorialManager/TutorialManager.h"
void UTutorialWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (NextButton)
	{
		NextButton->OnClicked.AddDynamic(this, &UTutorialWidget::NextbuttonPressed);
	}
	if (PreviousButton)
	{
		PreviousButton->OnClicked.AddDynamic(this, &UTutorialWidget::PreviousButtonPressed);
	}
}

void UTutorialWidget::SetText(FText text)
{
	TutorialText->SetText(text);
}

void UTutorialWidget::NextbuttonPressed()
{
	if (TutorialManager)
	{
		TutorialManager->AdvanceTutorial();
	}
}

void UTutorialWidget::PreviousButtonPressed()
{
	{
		TutorialManager->CurrentStepIndex--;
		TutorialManager->DisplayCurrentStep();
	}
}
