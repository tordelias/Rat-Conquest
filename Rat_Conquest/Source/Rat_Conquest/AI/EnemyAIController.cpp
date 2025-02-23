// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"

#include "Kismet/GameplayStatics.h"

#include "Rat_Conquest/Unit/PlayerUnit.h"
#include "Rat_Conquest/Managers/GridManager/GridManager.h"
#include "Rat_Conquest/GridTile/GridTile.h"
#include "Rat_Conquest/Managers/CombatManager/CombatManager.h"

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	UE_LOG(LogTemp, Error, TEXT("AEnemyAIController possessed pawn: %s"), *InPawn->GetName());
}

void AEnemyAIController::Tick(float DeltaTime)
{
}

void AEnemyAIController::MoveToGridPosition()
{
	UE_LOG(LogTemp, Error, TEXT("AI moving to grid position"));
	auto closestEnemy = this->FindEnemyunit();
	//cast to player unit
	APlayerUnit* AI = Cast<APlayerUnit>(GetPawn());

	if (IsValid(closestEnemy))
	{
		//if (!AI->bIsRangedUnit)
		//{
			float Distance = FVector2D::Distance(AI->CurrentGridPosition, closestEnemy->CurrentGridPosition);

			if (Distance < AI->MovementSpeed + 1)
			{
				UE_LOG(LogTemp, Error, TEXT("AI trying to Attack"));
				this->Attack(closestEnemy);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("AI moving to closest possible tile"));
				this->MoveToClosestPossibleTile(closestEnemy);
			}
		//}
		//else
		//{
		//	this->RandedAttack(closestEnemy);
		//}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No valid enemy found"));

	}
}

APlayerUnit* AEnemyAIController::FindEnemyunit()
{
	FVector2D closestUnitPos = FVector2D::ZeroVector;
	APlayerUnit* closestUnit = nullptr;

	APlayerUnit* AI = Cast<APlayerUnit>(GetPawn());

	if (!AI->GridManager)
	{
		UE_LOG(LogTemp, Error, TEXT("GridManager is not valid or has not been possessed"));
		return nullptr;
	}

	int numTilesChecked = 0;

	for (const auto& TilePair : AI->GridManager->GridTiles)
	{
		// Cast the AActor* value to AGridTile*
		AGridTile* Tile = Cast<AGridTile>(TilePair.Value);
		if (!Tile)
		{
			UE_LOG(LogTemp, Warning, TEXT("Invalid tile found in GridTiles"));
			continue;
		}
		APlayerUnit* Unit = Tile->unitRefrence;

		if (!Unit)
		{
			UE_LOG(LogTemp, Verbose, TEXT("Tile at position %s has no unit reference"), *TilePair.Key.ToString());
			continue;
		}

		if (Unit == AI)
		{
			UE_LOG(LogTemp, Verbose, TEXT("Skipping self"));
			continue;
		}

		if (!Unit->bIsPlayerUnit)
		{
			UE_LOG(LogTemp, Verbose, TEXT("Skipping enemy unit: %s"), *Unit->GetName());
			continue;

		}

		if (!closestUnit || FVector2D::Distance(AI->CurrentGridPosition, Unit->CurrentGridPosition) < FVector2D::Distance(AI->CurrentGridPosition, closestUnitPos))
		{
			closestUnitPos = Unit->CurrentGridPosition;
			closestUnit = Unit;
			UE_LOG(LogTemp, Log, TEXT("Found a closer FriendlyUnit: %s at location: %s"), *Unit->GetName(), *closestUnitPos.ToString());
		}

		numTilesChecked++;
	}

	if (!closestUnit)
	{
		UE_LOG(LogTemp, Warning, TEXT("No FriendlyUnit found in the grid after checking %d tiles"), numTilesChecked);
	}

	return closestUnit;
}

void AEnemyAIController::MoveToClosestPossibleTile(APlayerUnit* Enemy)
{

	APlayerUnit* AI = Cast<APlayerUnit>(GetPawn());
	if (AI->GridManager && Enemy)
	{
		FVector2D CurrentPosition = AI->CurrentGridPosition;
		FVector2D EnemyPosition = Enemy->CurrentGridPosition;
		 
		TArray<AGridTile*> PossibleTiles;
		PossibleTiles = AI->GridManager->GetNeighbourTiles(AI->CurrentGridPosition.X, AI->CurrentGridPosition.Y);

		AGridTile* BestTile = nullptr;
		float BestScore = FLT_MAX;

		for (AGridTile* Tile : PossibleTiles)
		{
			float DistanceToTile = FVector2D::Distance(CurrentPosition, Tile->GridPosition);

			if (DistanceToTile <= AI->MovementSpeed && !Tile->bIsOccupied)
			{
				float DistanceToEnemyFromTile = FVector2D::Distance(Tile->GridPosition, EnemyPosition);
				float TotalScore = DistanceToTile + DistanceToEnemyFromTile;

				if (TotalScore < BestScore)
				{
					BestScore = TotalScore;
					BestTile = Tile;
				}
			}
		}

		if (BestTile && !AI->bIsPlayerUnit)
		{
			AI->MoveToTile(BestTile->GridPosition);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No valid tile found within range"));

		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GridManager or Enemy is not valid"));

	}
}

void AEnemyAIController::Attack(APlayerUnit* Enemy)
{
	APlayerUnit* AI = Cast<APlayerUnit>(GetPawn());

	if (AI->GridManager && !AI->bIsPlayerUnit && Enemy)
	{
		FVector2D AIPosition = AI->CurrentGridPosition;
		FVector2D EnemyPosition = Enemy->CurrentGridPosition;

		TArray<AGridTile*> NeighbourTiles = AI->GridManager->GetNeighbourTiles(EnemyPosition.X, EnemyPosition.Y);

		AGridTile* BestTile = nullptr;
		float ClosestDistanceToEnemy = FLT_MAX;

		for (AGridTile* Tile : NeighbourTiles)
		{
			if (Tile && !Tile->bIsOccupied)
			{
				float DistanceToEnemy = FVector2D::Distance(Tile->GridPosition, EnemyPosition);

				if (DistanceToEnemy < ClosestDistanceToEnemy && Tile->GridPosition != EnemyPosition)
				{
					ClosestDistanceToEnemy = DistanceToEnemy;
					BestTile = Tile;
				}
			}
		}

		if (BestTile && Enemy->bIsPlayerUnit)
		{
			UE_LOG(LogTemp, Error, TEXT("AI moving to tile (%f, %f) to attack the enemy"), BestTile->GridPosition.X, BestTile->GridPosition.Y);
			AI->MoveToTile(BestTile->GridPosition); 
			AI->EnemyToAttack = Enemy;
			AI->OnMovementComplete.BindUObject(AI, &APlayerUnit::AttackAfterMovement);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("No valid tiles directly adjacent to the enemy. Moving closer instead."));
			MoveToClosestPossibleTile(Enemy);
		}
	}
	else
	{
		if (!AI->GridManager) UE_LOG(LogTemp, Error, TEXT("GridManager is invalid"));
		if (!Enemy) UE_LOG(LogTemp, Error, TEXT("Enemy is null"));
		if (AI->bIsPlayerUnit) UE_LOG(LogTemp, Warning, TEXT("Player-controlled units cannot use this AI logic"));
	}
}

void AEnemyAIController::RandedAttack(APlayerUnit* Enemy)
{
	APlayerUnit* AI = Cast<APlayerUnit>(GetPawn());
	//Attack the enemy

	AI->EnemyToAttack = Enemy;
	AI->AttackAfterMovement();
	AI->FinishTurn();
}
