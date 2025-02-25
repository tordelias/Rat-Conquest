// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitStatWidget.h"
#include "Components/TextBlock.h"
#include "Rat_Conquest/Components/InteractionInterface.h"

void UUnitStatWidget::UpdateWidget(const FInteractableData* InteractableData) const
{
    if (InteractableData)
    {
        if (UnitName) UnitName->SetText(FText::Format(NSLOCTEXT("UnitStats", "UnitName", "Unit Name: {0}"), InteractableData->UnitName));
        if (UnitHealth) UnitHealth->SetText(FText::FromString("Health: " + FString::FromInt(InteractableData->UnitHealth)));
        if (UnitDamage) UnitDamage->SetText(FText::FromString("Damage: " + FString::FromInt(InteractableData->UnitDamage)));
        if (UnitMovementSpeed) UnitMovementSpeed->SetText(FText::FromString("Speed: " + FString::FromInt(InteractableData->UnitMovementSpeed)));
        if (UnitDefense) UnitDefense->SetText(FText::FromString("Defense: " + FString::FromInt(InteractableData->Defense)));
        else UE_LOG(LogTemp, Error, TEXT("UnitDefense is nullptr!"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("InteractableData is nullptr"));
    }
}


