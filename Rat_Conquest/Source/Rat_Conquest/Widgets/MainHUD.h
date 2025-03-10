// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MainHUD.generated.h"

struct FInteractableData; 
class UUnitStatWidget; 
class UMainWidget;
class UMutationWidget;
class APlayerUnit;
class USelectRoomWidget;
class ALevelGenerator;
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget", meta = (AllowPrivate = "true"))
	TSubclassOf<UMainWidget> MainWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget", meta = (AllowPrivate = "true"))
	TSubclassOf<UMutationWidget> MutationWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget", meta = (AllowPrivate = "true"))
	TSubclassOf<USelectRoomWidget> RoomSelectWidgetClass;

	UPROPERTY()
	UUnitStatWidget* StatWidget;

	UPROPERTY()
	UMainWidget* MainWidget;

	UPROPERTY()
	UMutationWidget* MutationWidget;

	UPROPERTY()
	USelectRoomWidget* RoomWidget;


public: 


	void ShowStatWidget();
	void CloseStatWidget();
	void UpdateStatWidget(FInteractableData* data);
	void AddTurnImage(class APlayerUnit* unit);
	void RemoveTurnImage();
	void ShowMutationWidget();
	void CloseMutationWidget();
	void UpdateMutationWidget(TArray<int> statsC1, TArray<int> statsC2, TArray<int> statsC3, FName UnitName, APlayerUnit* unit);
	void ShowRoomSelectWidget();
	void CloseRoomSelectWidget();
	void SetupRoomSelectWidget(ALevelGenerator* _Levelgen);
	
};
