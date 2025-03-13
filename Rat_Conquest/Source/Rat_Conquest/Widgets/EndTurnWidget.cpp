// Fill out your copyright notice in the Description page of Project Settings.


#include "EndTurnWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Rat_Conquest/Managers/GameManager/GameManager.h"
#include "Rat_Conquest/Unit/PlayerUnit.h"

void UEndTurnWidget::NativeConstruct()
{
	Super::NativeConstruct();
	EndTurnButton->OnClicked.AddDynamic(this, &UEndTurnWidget::EndTurnButtonPressed);
	GameManager = Cast<AGameManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameManager::StaticClass()));
}

void UEndTurnWidget::EndTurnButtonPressed()
{
	if (GameManager)
	{
		if (GameManager->CurrentUnit && GameManager->CurrentUnit->bIsPlayerUnit)
		{
			GameManager->EndUnitTurn();
		}
		
	}
}
