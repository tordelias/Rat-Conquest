// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EndTurnWidget.generated.h"

class AGameManager;
class UButton;
/**
 * 
 */
UCLASS()
class RAT_CONQUEST_API UEndTurnWidget : public UUserWidget
{
	GENERATED_BODY()

public: 
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* EndTurnButton;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void EndTurnButtonPressed(); 

private:
	UPROPERTY()
	AGameManager* GameManager;
};
