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

UCLASS()
class RAT_CONQUEST_API AGameManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameManager();

	// Add a public function to toggle the turn between players
	void TogglePlayerTurn();

	void InitalizeUnits();

	void StartTurnOrder();

	void ExecuteTurn();

	void EndUnitTurn();

	void RemoveUnitFromQueue(APlayerUnit* unit);

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

	TArray<APlayerUnit*> MasterTurnQueue;
	const int32 TurnBufferSize = 10;

	int RoomsExplored = 0;

	bool hasSpawned = false;
	bool bLevelFinishedGenerating = false;
	bool bEncounterComplete = false;
	
	UPROPERTY()
	TArray<APlayerUnit*> PlayerUnits;

	UPROPERTY()
	TArray<APlayerUnit*> EnemyUnits;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	AGridManager* GridManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	ALevelGenerator* LevelGenerator;


	UPROPERTY()
	TArray<APlayerUnit*> TurnQueue;

	UPROPERTY(EditAnywhere, Category = "UI")
	int32 VisibleTurnsAhead = 15;
	
	UPROPERTY(EditAnywhere, Category = "Enemies")
	TArray<TSubclassOf<APlayerUnit>> EnemyList;

	UPROPERTY(EditAnywhere, Category = "Items")
	TArray<TSubclassOf<AItem>> ItemPool;

	UPROPERTY()
	APlayerUnit* CurrentUnit;

	UPROPERTY(EditAnywhere, Category = "Test")
	bool bTestEncounter = true;


	//sound
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound & Effects")
	USoundBase* SB_PlayerTurn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound & Effects")
	USoundBase* SB_EnemyTurn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound & Effects")
	USoundBase* SB_TurnExecuted;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	bool bisPlayersturn = true;

	TArray<class AGridTile*> CurrentlyFocusedTiles;
	APlayerUnit* CurrentlyFocusedUnit = nullptr;
	void HighlightUnitAndTiles(APlayerUnit* NewUnit); 

	AMainHUD* MainHUD;


	bool bIsGamePaused = false;



};
