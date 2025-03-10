// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TurnIndicatorWidget.generated.h"


class APlayerUnit; 

/**
 * 
 */
UCLASS()
class RAT_CONQUEST_API UTurnIndicatorWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void AddTurnImage(APlayerUnit* unit);
	UFUNCTION(BlueprintCallable)
	void RemoveTurnImage();
	UFUNCTION(BlueprintCallable)
	void ClearTurnImages();
	UFUNCTION(BlueprintCallable)
	void SetCurrentUnit(APlayerUnit* Unit);

	void UpdateTurnQueue(const TArray<APlayerUnit*>& sentTurnQueue);
	void RemoveUnitFromQueue(APlayerUnit* unit);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> UnitImageWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TArray<UUserWidget*> UnitImageWidgets;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* CurrentUnitImageWidget;

	TArray<APlayerUnit*> TurnQueue;
	
	
};
