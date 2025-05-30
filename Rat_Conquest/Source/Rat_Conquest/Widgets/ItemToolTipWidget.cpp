// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemToolTipWidget.h"
#include "Rat_Conquest/Items/Item.h"
#include "Rat_Conquest/Items/ItemBase.h"
#include "Rat_Conquest/Data/ItemDataStructs.h"
#include "Components/TextBlock.h"

void UItemToolTipWidget::SetData(AItem* _Item)
{
	if (_Item)
	{
		Item = _Item;
		// Set all stats = nullptr
		Attack->SetText(FText::FromString(""));
		Damage->SetText(FText::FromString(""));
		Range->SetText(FText::FromString(""));
		Health->SetText(FText::FromString(""));
		Defense->SetText(FText::FromString(""));
		Speed->SetText(FText::FromString(""));
		Initiative->SetText(FText::FromString(""));
		// Set the stats based on the item type

		if (Item->ItemDataB->ItemType == EItemType::Armor)
		{
			//Set Health, Defense, Speed
			Health->SetText(FText::FromString(FString::FromInt(Item->ItemDataB->ItemStatistics.MaxHealth)));
			Defense->SetText(FText::FromString(FString::FromInt(Item->ItemDataB->ItemStatistics.Defence)));
			Speed->SetText(FText::FromString(FString::FromInt(Item->ItemDataB->ItemStatistics.MovementSpeed)));

			Health->SetVisibility(ESlateVisibility::Visible);
			Defense->SetVisibility(ESlateVisibility::Visible);
			Speed->SetVisibility(ESlateVisibility::Visible);

			//Remove the rest of the stats from viewport
			Attack->SetVisibility(ESlateVisibility::Collapsed);
			Damage->SetVisibility(ESlateVisibility::Collapsed);
			Range->SetVisibility(ESlateVisibility::Collapsed);
			Initiative->SetVisibility(ESlateVisibility::Collapsed);
			
		}
		else if (Item->ItemDataB->ItemType == EItemType::Weapon)
		{
			//Set Damage, Attack, Range
			//SetText(FText::FromString("Damage: " + FString::FromInt(Unit->MinDamage) + "/" + FString::FromInt(Unit->MaxDamage)));
			Damage->SetText(FText::FromString(FString::FromInt(Item->ItemDataB->ItemStatistics.Damage)));
			Attack->SetText(FText::FromString(FString::FromInt(Item->ItemDataB->ItemStatistics.Attack)));
			Range->SetText(FText::FromString(FString::FromInt(Item->ItemDataB->ItemStatistics.Range)));

			Damage->SetVisibility(ESlateVisibility::Visible);
			Attack->SetVisibility(ESlateVisibility::Visible);
			Range->SetVisibility(ESlateVisibility::Visible);

			//Remove the rest of the stats from viewport
			Health->SetVisibility(ESlateVisibility::Collapsed);
			Defense->SetVisibility(ESlateVisibility::Collapsed);
			Speed->SetVisibility(ESlateVisibility::Collapsed);
			Initiative->SetVisibility(ESlateVisibility::Collapsed);
			
		}
		else if (Item->ItemDataB->ItemType == EItemType::Helmet)
		{
			//Initative, Attack, Health
			Damage->SetText(FText::FromString(FString::FromInt(Item->ItemDataB->ItemStatistics.Damage)));
			Attack->SetText(FText::FromString(FString::FromInt(Item->ItemDataB->ItemStatistics.Attack)));
			Range->SetText(FText::FromString(FString::FromInt(Item->ItemDataB->ItemStatistics.Range)));

			Damage->SetVisibility(ESlateVisibility::Visible);
			Attack->SetVisibility(ESlateVisibility::Visible);
			Range->SetVisibility(ESlateVisibility::Visible);

			//Remove the rest of the stats from viewport
			Health->SetVisibility(ESlateVisibility::Collapsed);
			Defense->SetVisibility(ESlateVisibility::Collapsed);
			Speed->SetVisibility(ESlateVisibility::Collapsed);
			Initiative->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{

		}

	}
}
