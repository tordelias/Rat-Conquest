// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitStatWidget.h"
#include "Components/TextBlock.h"
#include "Rat_Conquest/Components/InteractionInterface.h"

void UUnitStatWidget::UpdateWidget(const FInteractableData* InteractableData) const
{
    if (InteractableData)
    {
        UnitName->SetText(FText::Format(NSLOCTEXT("UnitStats", "UnitName", "Unit Name: {0}"), InteractableData->UnitName));

        UnitHealth->SetText(FText::FromString("Health: " + FString::FromInt(InteractableData->UnitHealth)));
        UnitDamage->SetText(FText::FromString("Damage: " + FString::FromInt(InteractableData->UnitDamage)));
        UnitMovementSpeed->SetText(FText::FromString("Speed: " + FString::FromInt(InteractableData->UnitMovementSpeed)));

		//UE_LOG(LogTemp, Log, TEXT("Updated widget with data"));
		//UE_LOG(LogTemp, Log, TEXT("Unit Name: %s"), *InteractableData->UnitName.ToString());
		//UE_LOG(LogTemp, Log, TEXT("Unit Health: %d"), InteractableData->UnitHealth);
		//UE_LOG(LogTemp, Log, TEXT("Unit Damage: %d"), InteractableData->UnitDamage);
  //      UE_LOG(LogTemp, Log, TEXT("Unit Movement Speed: %d"), InteractableData->UnitMovementSpeed);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("InteractableData is nullptr"));
    }
}

