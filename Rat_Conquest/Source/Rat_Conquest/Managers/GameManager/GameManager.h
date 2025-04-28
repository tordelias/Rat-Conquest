// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameManager.generated.h"


class APlayerUnit;
class AMainHUD;
class AGridManager;
class ALevelGenerator;
class AItem;
class AInteractableGridObject;

UCLASS()
class RAT_CONQUEST_API AGameManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGameManager();

	// Add a public function to toggle the turn between players
	void TogglePlayerTurn();

	UFUNCTION(BlueprintCallable, Category = "GameManager")
	void SetGameDifficulty(int difficulty);

	void InitalizeUnits();

	void StartTurnOrder();

	void ExecuteTurn();

	void EndUnitTurn();

	void RemoveUnitFromQueue(TObjectPtr<APlayerUnit> unit);

	void HandleAITurnAfterDelay();

	void EndEncounter();

	void CheckForEncounter();

	void LoadNextEncounter();

	void LoadExploredEncounter();

	void StartEncounter();

	void UpdateTurnQueue();

	void GenerateTurnBuffer();

	void PauseGame();
	void ResumeGame();

	bool isUnitAlive();

	void SpawnLoot();

	void SpawnInteractableObjects();

	void DelayedSpawnLoot();

	void DelayedSpawnInteractableObjects();

	void SpawnNewPlayerUnit(FVector2D Gridposition);

	void RotateUnits(float roation);

	void RotateEnemyUnits();

	TArray<TObjectPtr<APlayerUnit>> MasterTurnQueue;
	const int32 TurnBufferSize = 10;

	int MaxRoomsToExplore = 5;
	int RoomsExplored = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int GameDifficulty = 0;

	bool hasSpawned = false;
	bool bLevelFinishedGenerating = false;
	bool bEncounterComplete = false;

	UPROPERTY()
	TArray<TObjectPtr<APlayerUnit>> PlayerUnits;

	UPROPERTY()
	TArray<TObjectPtr<APlayerUnit>> EnemyUnits;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	TObjectPtr<AGridManager> GridManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	TObjectPtr<ALevelGenerator> LevelGenerator;

	UPROPERTY()
	TArray<TObjectPtr<APlayerUnit>> TurnQueue;

	UPROPERTY(EditAnywhere, Category = "UI")
	int32 VisibleTurnsAhead = 15;

	UPROPERTY(EditAnywhere, Category = "Enemies")
	TArray<TSubclassOf<APlayerUnit>> EnemyList;

	UPROPERTY(EditAnywhere, Category = "Enemies")
	TArray<TSubclassOf<APlayerUnit>> BossList;

	UPROPERTY(EditAnywhere, Category = "Player Units")
	TArray<TSubclassOf<APlayerUnit>> NewPlayerUnitList;

	UPROPERTY(EditAnywhere, Category = "Items")
	TArray<TSubclassOf<AItem>> ItemPool;
	

	UPROPERTY(EditAnywhere, Category = "Grid Objects")
	TArray<TSubclassOf<AInteractableGridObject>> InteractObjPool;

	UPROPERTY()
	TObjectPtr<APlayerUnit> CurrentUnit;

	UPROPERTY(EditAnywhere, Category = "Test")
	bool bTestEncounter = true;

	//sound
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound & Effects")
	TObjectPtr<USoundBase> SB_PlayerTurn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound & Effects")
	TObjectPtr<USoundBase> SB_EnemyTurn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound & Effects")
	TObjectPtr<USoundBase> SB_TurnExecuted;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	bool bisPlayersturn = true;

	TArray<TObjectPtr<class AGridTile>> CurrentlyFocusedTiles;
	TObjectPtr<APlayerUnit> CurrentlyFocusedUnit = nullptr;
	void HighlightUnitAndTiles(APlayerUnit* NewUnit);
	void MarkCurrentUnit(); 

	TObjectPtr<AMainHUD> MainHUD;

	FTimerHandle CameraDelayTimerHandle;

	UPROPERTY()
	TObjectPtr<class AUnitMarker> unitMarker;

	bool bIsGamePaused = false;
};
