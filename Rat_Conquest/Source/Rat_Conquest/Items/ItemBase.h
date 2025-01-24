// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Rat_Conquest/Data/ItemDataStructs.h"
#include "ItemBase.generated.h"

/**
 * 
 */
UCLASS()
class RAT_CONQUEST_API UItemBase : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, Category = "Item")
	int32 Quantity;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	FName ID;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	EItemType ItemType;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	EItemQuality ItemQuality;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	FItemStatistics ItemStatistics;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	FItemTextData ItemTextData;


	UPROPERTY(VisibleAnywhere, Category = "Item")
	FItemAssetData ItemAssetData;

	bool bIsCopy;
	bool bIsPickup;

	UItemBase();

	void ResetItemFlags();

	UItemBase* CreateItemCopy() const;
	

	UFUNCTION(Category = "Item")
	void SetQuantity(const int32 NewQuantity);

	//virtual void Use(ACharacter* Character);

protected:
	bool operator == (const FName& OtherID) const
	{
		return this->ID == OtherID;
	}
};
