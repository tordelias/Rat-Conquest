// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MainHUD.generated.h"

struct FInteractableData; 
class UUnitStatWidget; 
/**
 * 
 */
UCLASS()
class RAT_CONQUEST_API AMainHUD : public AHUD
{
	GENERATED_BODY()

protected: 
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUnitStatWidget> UnitWidgetclass;

	UPROPERTY()
	UUnitStatWidget* StatWidget;

public: 

	void ShowStatWidget();
	void CloseStatWidget();
	void UpdateStatWidget(FInteractableData* data);
	
};
