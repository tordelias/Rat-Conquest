// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitStatWidget.h"
#include "Components/TextBlock.h"
#include "Rat_Conquest/Components/InteractionInterface.h"

void UUnitStatWidget::UpdateWidget(const FInteractableData* InteractableData) const
{
    if (InteractableData)
    {
        if (UnitName) UnitName->SetText(FText::Format(NSLOCTEXT("UnitStats", "UnitName", "{0}"), InteractableData->UnitName));
        //write health as health/maxHealth
		if (UnitHealth) UnitHealth->SetText(FText::FromString("Health: " + FString::FromInt(InteractableData->UnitHealth) + "/" + FString::FromInt(InteractableData->maxHealth)));
        if (UnitDamage) UnitDamage->SetText(FText::FromString("Damage: " + FString::FromInt(InteractableData->MinDamage) + "/" + FString::FromInt(InteractableData->MaxDamage)));
        if (UnitMovementSpeed) UnitMovementSpeed->SetText(FText::FromString("Speed: " + FString::FromInt(InteractableData->UnitMovementSpeed)));
        if (UnitDefense) UnitDefense->SetText(FText::FromString("Defense: " + FString::FromInt(InteractableData->Defense)));
		if (UnitAttack) UnitAttack->SetText(FText::FromString("Attack: " + FString::FromInt(InteractableData->Attack)));
		if (UnitInitative) UnitInitative->SetText(FText::FromString("Initiative: " + FString::FromInt(InteractableData->Initative)));
		if (UnitRange) UnitRange->SetText(FText::FromString("Range: " + FString::FromInt(InteractableData->Range)));
        else UE_LOG(LogTemp, Error, TEXT("UnitDefense is nullptr!"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("InteractableData is nullptr"));
    }
}


