// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameManager.generated.h"


class APlayerUnit;
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

	void StartEncounter();

	UPROPERTY()
	TArray<APlayerUnit*> PlayerUnits;

	UPROPERTY()
	TArray<APlayerUnit*> EnemyUnits;


	UPROPERTY()
	TArray<APlayerUnit*> TurnQueue;


	
	UPROPERTY(EditAnywhere, Category = "Enemies")
	TArray<TSubclassOf<APlayerUnit>> EnemyList;

	UPROPERTY()
	APlayerUnit* CurrentUnit;



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
};
