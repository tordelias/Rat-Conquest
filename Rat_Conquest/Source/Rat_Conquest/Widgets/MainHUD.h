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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget", meta = (AllowPrivate = "true"))
	TSubclassOf<UUserWidget> GameOverWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget", meta = (AllowPrivate = "true"))
	TSubclassOf<UUserWidget> VictoryWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget", meta = (AllowPrivate = "true"))
	TSubclassOf<UUserWidget> PausescreenWidgetClass;

	UPROPERTY()
	UUnitStatWidget* StatWidget;

	UPROPERTY()
	UMainWidget* MainWidget;

	UPROPERTY()
	UMutationWidget* MutationWidget;

	UPROPERTY()
	USelectRoomWidget* RoomWidget;

	UPROPERTY()
	UUserWidget* GameOverWidget;

	UPROPERTY()
	UUserWidget* VictoryWidget;

	UPROPERTY()
	UUserWidget* PauseScreenWidget; 

public:
	//MainWidget functions
	UFUNCTION(BlueprintCallable)
	void ShowMainWidget();
	void AddTurnImage(class APlayerUnit* unit);
	void RemoveTurnImage();
	void ClearTurnImages();
	void UpdateTurnQueue(const TArray<APlayerUnit*>& TurnQueue);
	void RemoveUnitFromQueue(APlayerUnit* unit);
	void AddPlayerUnitToScreen(APlayerUnit* unit);
	void HighlightPlayerUnit(TWeakObjectPtr<APlayerUnit> unit);

	//statWidget functions
	void ShowStatWidget();
	void CloseStatWidget();
	void UpdateStatWidget(FInteractableData* data);

	//MutationWidget functions
	void ShowMutationWidget();
	void CloseMutationWidget();
	void UpdateMutationWidget(TArray<int> statsC1, TArray<int> statsC2, TArray<int> statsC3, FName UnitName, APlayerUnit* unit);

	//RoomSelectWidget functions
	void ShowRoomSelectWidget();
	void CloseRoomSelectWidget();
	void SetupRoomSelectWidget(ALevelGenerator* _Levelgen);
	void GetRoomSelectDoors(TArray<bool> _DoorDirections);

	//Game over widget
	void ShowGameOverWidget();
	void CloseGameOverWidget();

	//Victory widget
	void ShowVictoryWidget();
	void CloseVictoryWidget();

	//Paucescreen widget
	void ShowPausescreenWidget();
	void ClosePausescreenWidget();

	bool IsPauseScreenWidgetVissible();
	
};