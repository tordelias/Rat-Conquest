// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UnitStatWidget.generated.h"

struct FInteractableData;
/**
 * 
 */
UCLASS()
class RAT_CONQUEST_API UUnitStatWidget : public UUserWidget
{
	GENERATED_BODY()
	

public:
	void UpdateWidget(const FInteractableData* InteractableData) const;

protected: 
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Unit Stat Widget")
	class UTextBlock* UnitName;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Unit Stat Widget")
	class UTextBlock* UnitHealth;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Unit Stat Widget")
	class UTextBlock* UnitDamage;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Unit Stat Widget")
	class UTextBlock* UnitAttack;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Unit Stat Widget")
	class UTextBlock* UnitInitative;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Unit Stat Widget")
	class UTextBlock* UnitRange;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Unit Stat Widget")
	class UTextBlock* UnitMovementSpeed;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Unit Stat Widget")
	class UTextBlock* UnitDefense;

};
