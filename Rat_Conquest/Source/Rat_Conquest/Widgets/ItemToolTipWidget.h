// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemToolTipWidget.generated.h"


class UTextBlock;
class AItem;
/**
 * 
 */
UCLASS()
class RAT_CONQUEST_API UItemToolTipWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* Attack;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* Damage;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* Range;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* Health;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* Defense;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* Speed;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* Initiative;

	UPROPERTY()
	AItem* Item;

	void SetData(AItem* _Item);

	
};
