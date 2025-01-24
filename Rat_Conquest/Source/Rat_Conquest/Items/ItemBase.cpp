// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemBase.h"

UItemBase::UItemBase()
{
    
}

void UItemBase::ResetItemFlags()
{
}

UItemBase* UItemBase::CreateItemCopy() const
{
    UItemBase* NewItem = NewObject<UItemBase>();
    // Copy data from the current item
    NewItem->ID = this->ID;
    NewItem->ItemType = this->ItemType;
    NewItem->ItemQuality = this->ItemQuality;
    NewItem->ItemStatistics = this->ItemStatistics;
    NewItem->ItemTextData = this->ItemTextData;
    NewItem->ItemAssetData = this->ItemAssetData;
    NewItem->Quantity = this->Quantity;

    // Return the new item
    return NewItem;
}

void UItemBase::SetQuantity(const int32 NewQuantity)
{
}
