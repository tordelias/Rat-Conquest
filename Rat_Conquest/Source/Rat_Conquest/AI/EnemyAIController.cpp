// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"

#include "Kismet/GameplayStatics.h"

#include "Rat_Conquest/Unit/PlayerUnit.h"
#include "Rat_Conquest/Managers/GridManager/GridManager.h"
#include "Rat_Conquest/GridTile/GridTile.h"
#include "Rat_Conquest/Managers/GameManager/GameManager.h"
#include "Rat_Conquest/Managers/CombatManager/CombatManager.h"
#include "Rat_Conquest/Items/Item.h"
#include "Rat_Conquest/Unit/GeneralAIUnit.h"

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AEnemyAIController::InitializeDifficulty, 3.f, false);

}

void AEnemyAIController::Tick(float DeltaTime)
{
}

void AEnemyAIController::InitializeDifficulty()
{
	AGeneralAIUnit* AI = Cast<AGeneralAIUnit>(GetPawn());
	if (AI)
	{

		FString DebugMessage = FString::Printf(TEXT("AI Difficulty: %d"), AI->AIDifficulty);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, DebugMessage);


		switch (AI->AIDifficulty)
		{
		case 0: // Easy
			Difficulty = EEnemyAIDifficulty::Easy;
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("AI Difficulty: Easy"));
			break;
		case 1: // Normal
			Difficulty = EEnemyAIDifficulty::Normal;
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("AI Difficulty: Normal"));
			break;
		case 2: // Hard
			Difficulty = EEnemyAIDifficulty::Hard;
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI Difficulty: Hard"));
			break;
		default:
			Difficulty = EEnemyAIDifficulty::Easy;
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI Difficulty: Default (Easy)"));
			break;
		}
	}
}


void AEnemyAIController::MoveToClosestPossibleTile(TWeakObjectPtr<APlayerUnit> Enemy)
{
	APlayerUnit* AI = Cast<APlayerUnit>(GetPawn());
	if (!AI || !AI->GridManager.IsValid() || !Enemy.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid references in MoveToClosestPossibleTile"));
		return;
	}

	FVector2D CurrentPosition = AI->CurrentGridPosition;
	FVector2D EnemyPosition = Enemy->CurrentGridPosition;
	const int32 MovementRange = AI->MovementSpeed;
	TArray<TWeakObjectPtr<AGridTile>> PossibleTiles = AI->GridManager->GetMovableTiles(CurrentPosition.X, CurrentPosition.Y, MovementRange);

	AGridTile* BestTile = nullptr;
	float BestDistanceToEnemy = FLT_MAX;

	for (auto Tile : PossibleTiles)
	{
		if (Tile->bIsOccupied && (!Tile->unitRefrence.IsValid() || Tile->unitRefrence->Health <= 0))
		{
			// Treat as not occupied if it's a ghost
			Tile->bIsOccupied = false;
			Tile->unitRefrence = nullptr;
		}
		if (!Tile.IsValid() || Tile->bIsOccupied)
			continue;
		float DistanceToEnemy = AI->ChebyshevDistance(Tile->GridPosition, EnemyPosition);
		if (DistanceToEnemy < BestDistanceToEnemy)
		{
			BestDistanceToEnemy = DistanceToEnemy;
			BestTile = Tile.Get();
		}
	}

	if (BestTile)
	{
		AI->MoveToTile(BestTile->GridPosition);
		return;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No reachable position found near enemy |EnemyAiController.cpp|"));
		AI->FinishTurn();
	}
}

void AEnemyAIController::MoveToMostTacticalTile(TWeakObjectPtr<APlayerUnit> Enemy)
{
	APlayerUnit* AI = Cast<APlayerUnit>(GetPawn());
	if (!AI || !AI->GridManager.IsValid() || !Enemy.IsValid())
		return;
	if (!bIsRanged)
	{
		MoveToClosestPossibleTile(Enemy);
		return;
	}

	FVector2D CurrentPosition = AI->CurrentGridPosition;
	FVector2D EnemyPosition = Enemy->CurrentGridPosition;

	TArray<TWeakObjectPtr<AGridTile>> PossibleTiles = AI->GridManager->GetMovableTiles(CurrentPosition.X, CurrentPosition.Y, AI->MovementSpeed);
	AGridTile* BestTile = nullptr;
	float BestScore = FLT_MAX;


	for (auto Tile : PossibleTiles)
	{
		if (Tile->bIsOccupied && (!Tile->unitRefrence.IsValid() || Tile->unitRefrence->Health <= 0))
		{
			// Treat as not occupied if it's a ghost
			Tile->bIsOccupied = false;
			Tile->unitRefrence = nullptr;
		}
		if (!Tile.IsValid() || Tile->bIsOccupied)
			continue;

		float DistanceToEnemy = AI->ChebyshevDistance(Tile->GridPosition, EnemyPosition);

		float TotalScore = 0.f;

		if (bIsRanged)
		{
			// Prefer keeping distance but staying in range (e.g., range = 3)
			const float IdealDistance = AI->AttackRange;
			TotalScore = FMath::Abs(IdealDistance - DistanceToEnemy); // Minimize distance deviation from ideal range
		}
		else
		{
		}

		if (TotalScore < BestScore)
		{
			BestScore = TotalScore;
			BestTile = Tile.Get();
		}
	}

	if (BestTile)
	{
		AI->MoveToTile(BestTile->GridPosition);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No reachable position found near enemy |EnemyAiController.cpp|"));
		AI->FinishTurn();
		}
}


void AEnemyAIController::ChooseAction()
{
	this->Target = nullptr;
	switch (Difficulty)
	{
	case EEnemyAIDifficulty::Easy:
		this->Target = FindClosestEnemy();
		break;
	case EEnemyAIDifficulty::Normal:
		this->Target = FindEnemyByThreat();
		break;
	case EEnemyAIDifficulty::Hard:
		this->Target = FindMostVulnerableEnemy();
		break;
	}
	//Log the target
	if (this->Target.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Target found: %s"), *this->Target->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No target found"));
		return;
	}
	if (bIsRanged)
	{
		RangedAttack();
	}
	else
	{
		MeleeAttack();
	}

}

void AEnemyAIController::MeleeAttack()
{
	APlayerUnit* AI = Cast<APlayerUnit>(GetPawn());
	if (AI->GridManager.IsValid() && Target.IsValid())
	{
		FVector2D AIPosition = AI->CurrentGridPosition;
		FVector2D EnemyPosition = Target->CurrentGridPosition;
		float DistanceToEnemy = AI->ChebyshevDistance(AIPosition, EnemyPosition);

		// If the AI can reach the target within its movement range
		if (DistanceToEnemy <= AI->MovementSpeed + 1)
		{
			AI->EnemyToAttack = Target;
			TArray<TWeakObjectPtr<AGridTile>> Neighbors = AI->GridManager->GetNeighbourTiles(Target->CurrentGridPosition.X, Target->CurrentGridPosition.Y);
			switch (Difficulty)
			{
			case EEnemyAIDifficulty::Easy:
				
				if (DistanceToEnemy == 1)
				{
					AI->AttackAfterMovement();
					return;
				}
				AI->OnMovementComplete.BindUObject(AI, &APlayerUnit::AttackAfterMovement);
				MoveToClosestPossibleTile(Target);
				break;

			case EEnemyAIDifficulty::Normal:
				if (FMath::RandRange(0, 1) == 0)
				{
					ScoreMeleeTiles(Neighbors, Target);
					if (TileOptions.Num() > 0 && TileOptions[0].Tile)
					{
						
						// Check if AI is not on the same tile it's already occupying
						if (TileOptions[0].Tile == AI->GridManager->GetTileAtPosition(AI->CurrentGridPosition.X, AI->CurrentGridPosition.Y).Get())
						{
							AI->AttackAfterMovement();
							return;
						}
						AI->OnMovementComplete.BindUObject(AI, &APlayerUnit::AttackAfterMovement);
						AI->MoveToTile(TileOptions[0].Tile->GridPosition);
						return;
					}
					else
					{
						if (DistanceToEnemy == 1)
						{
							AI->AttackAfterMovement();
							return;
						}
						AI->OnMovementComplete.BindUObject(AI, &APlayerUnit::AttackAfterMovement);
						MoveToClosestPossibleTile(Target);
					}
				}
				else
				{
					AI->OnMovementComplete.BindUObject(AI, &APlayerUnit::AttackAfterMovement);
					MoveToClosestPossibleTile(Target);
				}
				break;

			case EEnemyAIDifficulty::Hard:
				ScoreMeleeTiles(Neighbors, Target);
				if (TileOptions.Num() > 0 && TileOptions[0].Tile)
				{
					
					// Check if AI is not on the same tile it's already occupying
					if (TileOptions[0].Tile == AI->GridManager->GetTileAtPosition(AI->CurrentGridPosition.X, AI->CurrentGridPosition.Y).Get())
					{
						AI->AttackAfterMovement();
						return;
					}
					AI->OnMovementComplete.BindUObject(AI, &APlayerUnit::AttackAfterMovement);
					AI->MoveToTile(TileOptions[0].Tile->GridPosition);
					return;
				}
				else
				{
					if (DistanceToEnemy == 1)
					{
						
						AI->AttackAfterMovement();
						return;
					}
					AI->OnMovementComplete.BindUObject(AI, &APlayerUnit::AttackAfterMovement);
					MoveToClosestPossibleTile(Target);
				}
				break;
			}

			return;
		}

		// If the AI is too far from the target, make a move based on difficulty
		switch (Difficulty)
		{
		case EEnemyAIDifficulty::Easy:
			MoveToClosestPossibleTile(Target);
			break;
		case EEnemyAIDifficulty::Normal:
			if (FMath::RandRange(0, 1) == 0)
				MoveToClosestPossibleTile(Target);
			else
				MoveToMostTacticalTile(Target);
			break;
		case EEnemyAIDifficulty::Hard:
			MoveToMostTacticalTile(Target);
			UE_LOG(LogTemp, Warning, TEXT("AI is too far from target, moving to tactical tile"));
			break;
		}
	}
	else
	{
		if (!AI->GridManager.IsValid()) UE_LOG(LogTemp, Error, TEXT("GridManager is invalid"));
		if (!Target.IsValid()) UE_LOG(LogTemp, Error, TEXT("Target is null"));
	}
}



void AEnemyAIController::RangedAttack()
{
	APlayerUnit* AI = Cast<APlayerUnit>(GetPawn());
	if (AI->GridManager.IsValid() && Target.IsValid())
	{
		FVector2D AIPosition = AI->CurrentGridPosition;
		FVector2D EnemyPosition = Target->CurrentGridPosition;
		float DistanceToEnemy = AI->ChebyshevDistance(AIPosition, EnemyPosition);
		if (DistanceToEnemy <= AI->AttackRange)
		{
			AI->EnemyToAttack = Target;
			AI->AttackAfterMovement();
			AI->ShootProjectile(Target->GetActorLocation());
			AI->FinishTurn();
		}
		else
		{
			switch (Difficulty)
			{
			case EEnemyAIDifficulty::Easy:
				MoveToClosestPossibleTile(Target);
				break;
			case EEnemyAIDifficulty::Normal:
				if (FMath::RandRange(0, 1) == 0)
				{
					MoveToClosestPossibleTile(Target);
				}
				else
				{
					MoveToMostTacticalTile(Target);
				}
				break;
			case EEnemyAIDifficulty::Hard:
				MoveToMostTacticalTile(Target);
				break;
			}
		
		}
	}
	else
	{
		if (!AI->GridManager.IsValid()) UE_LOG(LogTemp, Error, TEXT("GridManager is invalid"));
		if (!Target.IsValid()) UE_LOG(LogTemp, Error, TEXT("Target is null"));
	}
}

TWeakObjectPtr<APlayerUnit> AEnemyAIController::FindClosestEnemy()
{
	//Find the closest enemy unit or tile closest to the playerunit 
	APlayerUnit* AI = Cast<APlayerUnit>(GetPawn());
	if (!AI->GridManager.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("GridManager is not valid or has not been possessed"));
		return nullptr;
	}

	if (AI->bIsRangedUnit)
	{
		bIsRanged = true;
	}
	else
	{
		bIsRanged = false;
	}

	TWeakObjectPtr<APlayerUnit> ClosestEnemy = nullptr;
	float ClosestDistance = FLT_MAX;
		for (const auto& TilePair : AI->GridManager->GridTiles)
		{
			AGridTile* Tile = Cast<AGridTile>(TilePair.Value);
			if (!Tile)
			{
				continue;
			}
			APlayerUnit* Unit = Tile->unitRefrence.Get();
			if (!Unit || Unit == AI || !Unit->bIsPlayerUnit)
			{
				continue;
			}
			float Distance = AI->ChebyshevDistance(AI->CurrentGridPosition, Unit->CurrentGridPosition);
			if (Distance < ClosestDistance)
			{
				ClosestDistance = Distance;
				ClosestEnemy = Unit;
			}
		}
		if (ClosestEnemy.IsValid())
			return ClosestEnemy;
		else
			return nullptr; 
}

TWeakObjectPtr<APlayerUnit> AEnemyAIController::FindEnemyByThreat()
{

	APlayerUnit* AI = Cast<APlayerUnit>(GetPawn());
	if (AI->bIsRangedUnit)
	{
		bIsRanged = true;
	}
	else
	{
		bIsRanged = false;
	}
	if (AI->GridManager.IsValid())
	{
		TWeakObjectPtr<APlayerUnit> MostThreateningEnemy;
		float HighestThreatLevel = -FLT_MAX; // Start with the lowest possible value to find the max threat

		// Iterate over all potential enemies
		for (TObjectIterator<APlayerUnit> It; It; ++It)
		{
			APlayerUnit* Enemy = *It;
			if (Enemy && Enemy != AI && Enemy->bIsPlayerUnit)
			{
				float Distance = AI->ChebyshevDistance(AI->CurrentGridPosition, Enemy->CurrentGridPosition);
				float ThreatLevel = 0.0f;

				ThreatLevel += 100 / (Distance + 1); 

				ThreatLevel += Enemy->Damage * (1 + Enemy->Attack / 10); 
					if (Enemy->Weapon.IsValid())
					{
						ThreatLevel += Enemy->Weapon->Damage;
					}

				if (ThreatLevel > HighestThreatLevel)
				{
					HighestThreatLevel = ThreatLevel;
					MostThreateningEnemy = Enemy;
				}
			}
		}

		return MostThreateningEnemy;
	}

	// If no valid AI or grid manager, return null
	return TWeakObjectPtr<APlayerUnit>();
}

TWeakObjectPtr<APlayerUnit> AEnemyAIController::FindMostVulnerableEnemy()
{
	APlayerUnit* AI = Cast<APlayerUnit>(GetPawn());
	if (AI->bIsRangedUnit)
	{
		bIsRanged = true;
	}
	else
	{
		bIsRanged = false;
	}
	if (AI->GridManager.IsValid())
	{
		TWeakObjectPtr<AGameManager> gameManager = AI->GameManager; 
		if (!gameManager.IsValid())
		{
			UE_LOG(LogTemp, Error, TEXT("GameManager is not valid or has not been possessed"));
			return nullptr;
		}
		TArray<TObjectPtr<APlayerUnit>> playerUnits = gameManager->PlayerUnits;
		TWeakObjectPtr<APlayerUnit> MostVulnerableEnemy;
		float LowestHealth = FLT_MAX;

		// Iterate over all potential enemies
		for (auto Enemy : playerUnits)
		{
			if (!Enemy)
				continue;

			if (Enemy && Enemy != AI && Enemy->bIsPlayerUnit)
			{
				float Health = Enemy->Health; 
				if (Health < LowestHealth)
				{
					LowestHealth = Health;
					MostVulnerableEnemy = Enemy;
				}
			}
		}

		return MostVulnerableEnemy;
	}
	return TWeakObjectPtr<APlayerUnit>();
}


void AEnemyAIController::ScoreMeleeTiles(TArray<TWeakObjectPtr<AGridTile>> NeighbourTiles, TWeakObjectPtr<APlayerUnit> Enemy)
{
	APlayerUnit* AI = Cast<APlayerUnit>(GetPawn());
	if (!AI->GridManager.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("GridManager is not valid or has not been possessed"));
		return;
	}
	TileOptions.Empty();
	for (auto& Tile : NeighbourTiles)
	{
		if (!Tile.IsValid() || Tile->bIsOccupied) continue;

		float Score = 0;

		//  Tiles with adjacent Tiles occupied by envirement
			TArray<TWeakObjectPtr<AGridTile>> TilesToCheck = AI->GridManager->GetMovableTiles(Tile->GridPosition.X, Tile->GridPosition.Y, AI->MovementSpeed);
			int EnemyCount = 0; 
			for (auto& AdjTile : TilesToCheck)
			{
				if (AdjTile.IsValid() && AdjTile->bIsOccupied && AdjTile->unitRefrence == nullptr)
				{
					Score -= 10;
				}
				else if (AdjTile.IsValid() && AdjTile->bIsOccupied && AdjTile->unitRefrence != nullptr)
				{
					EnemyCount++;
				}

			}
			Score += EnemyCount * 10; // Higher = worse

			if (TilesToCheck.Num() < 3)
			{
				Score += 15; //corner Tile
			}

		TileOptions.Add({ Tile.Get(), Score });
	}
	// Sort the tile options based on score lower is better
	TileOptions.Sort([](const FMeleeTileOption& A, const FMeleeTileOption& B)
		{
			return A.Score < B.Score;
		});
}

