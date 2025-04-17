// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

class APlayerUnit; 
class AGridTile;

UENUM(BlueprintType)
enum class EEnemyAIDifficulty : uint8
{
	Easy UMETA(DisplayName = "Easy"),
	Normal UMETA(DisplayName = "Normal"),
	Hard UMETA(DisplayName = "Hard")
};
struct FMeleeTileOption
{
	AGridTile* Tile;
	float Score;
};

/**
 * 
 */
UCLASS()
class RAT_CONQUEST_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	EEnemyAIDifficulty Difficulty = EEnemyAIDifficulty::Normal;


	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;

	void InitializeDifficulty();

	void MoveToClosestPossibleTile(TWeakObjectPtr<APlayerUnit> Enemy);
	void MoveToMostTacticalTile(TWeakObjectPtr<APlayerUnit> Enemy);

	virtual void ChooseAction();
	virtual void MeleeAttack();
	void RangedAttack();

	TWeakObjectPtr<APlayerUnit> FindClosestEnemy();
	TWeakObjectPtr<APlayerUnit> FindEnemyByThreat();
	TWeakObjectPtr<APlayerUnit> FindMostVulnerableEnemy();


	UPROPERTY()
	TWeakObjectPtr<APlayerUnit> Target;

private: 
	FVector2d TargetGridPosition;

	TArray<FMeleeTileOption> TileOptions;
	void ScoreMeleeTiles(TArray<TWeakObjectPtr<AGridTile>> NeighbourTiles, TWeakObjectPtr<APlayerUnit> Enemy);

	bool bIsRanged = false; 
	
};
