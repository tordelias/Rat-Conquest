// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerUnit.h"

//Engine

//includes
#include "Rat_Conquest/GridManager/GridManager.h"
#include "Rat_Conquest/Player/PlayerCamera.h"
#include "Rat_Conquest/GameManager/GameManager.h"
#include "Rat_Conquest/GridTile/GridTile.h"
#include "Rat_Conquest/CombatManager/CombatManager.h"
#include "Kismet/GameplayStatics.h" // Include for UGameplayStatics

// Sets default values
APlayerUnit::APlayerUnit()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	UpdateInteractableData();

	GridStartPosition = FVector2D(0, 0);
}

// Called when the game starts or when spawned
void APlayerUnit::BeginPlay()
{
	Super::BeginPlay();

	//if (bIsPlayerUnit) {
	//	DelayedInitalPosition();
	//}
	DelayedInitalPosition();
}
// Called every frame
void APlayerUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsMoving)
	{
		MovementProgress += DeltaTime / MovementDuration;

		if (MovementProgress >= 1.0f)
		{
			MovementProgress = 1.0f;
			bIsMoving = false;
			SetActorLocation(TargetPosition);
			UE_LOG(LogTemp, Display, TEXT("Finished moving to new tile: %s"), *TargetPosition.ToString());
		}
		else
		{
			FVector NewPosition = FMath::Lerp(StartPosition, TargetPosition, MovementProgress);
			SetActorLocation(NewPosition);
		}
	}
	if (GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::E))
	{
		TArray<AGridTile*> listofTiles = GridManager->GetNeighbourTiles(CurrentGridPosition.X,CurrentGridPosition.Y);


	}
	if (GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::Q))
	{
		if(!bIsPlayerUnit)
		{
			this->MoveToGridPosition();
		}
	}
}

void APlayerUnit::MoveToTile(FVector2D NewGridPosition)
{
	if (!GridManager) {
		UE_LOG(LogTemp, Error, TEXT("NO MANAGER"));
		return;
	}

	AActor* TargetTile = GridManager->GetTileAt(NewGridPosition.X, NewGridPosition.Y);
	if (!TargetTile)
		return;

	AGridTile* GridTile = Cast<AGridTile>(TargetTile);
	if (!GridTile || GridTile->bIsOccupied)
		return;

	AActor* OldTile = GridManager->GetTileAt(CurrentGridPosition.X, CurrentGridPosition.Y);
	AGridTile* OldGridTile = Cast<AGridTile>(OldTile);
	if (OldTile) {
		if (OldGridTile) {
			OldGridTile->bIsOccupied = false;
			
		} 
			
	}
	

	if (GridManager->GetDistanceBetweenTiles(TargetTile, OldTile) > movementSpeed)
		return;
	OldGridTile->RemoveUnitRefrence();
	
	// Mark new tile as occupied
	GridTile->bIsOccupied = true;
	GridTile->SetUnitRefrence(this);
	// Set up lerp variables
	StartPosition = GetActorLocation();
	TargetPosition = TargetTile->GetActorLocation();
	MovementProgress = 0.0f;  // Reset progress
	bIsMoving = true;         // Start movement

	CurrentGridPosition = NewGridPosition;
	UE_LOG(LogTemp, Display, TEXT("Started moving to new tile."));
	FinishTurn();
}

void APlayerUnit::SetInitalPosition(FVector2D position)
{
	if (!GridManager) {
		UE_LOG(LogTemp, Error, TEXT("NO MANAGER"));
		return;
	}

	AActor* TargetTile = GridManager->GetTileAt(position.X, position.Y);
	if (!TargetTile) {
		UE_LOG(LogTemp, Warning, TEXT("Invalid target tile at (%f, %f)"), position.X, position.Y);
		return;
	}

	TargetPosition = TargetTile->GetActorLocation();
	SetActorLocation(TargetPosition);
	CurrentGridPosition = position;
	//set the unit pointer on the grid tile
	AGridTile* GridTile = Cast<AGridTile>(TargetTile);
	if (GridTile) {
		GridTile->SetUnitRefrence(this);
		GridTile->bIsOccupied = true;
	}
	UE_LOG(LogTemp, Display, TEXT("Player unit set to grid position (%f, %f)"), position.X, position.Y);
}

void APlayerUnit::DelayedInitalPosition()
{
	if (GridManager && GridManager->bIsGridFinished())
	{
		SetInitalPosition(GridStartPosition);
	}
	else
	{
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimerForNextTick(this, &APlayerUnit::DelayedInitalPosition);
	}
}

void APlayerUnit::PlayerAttack(APlayerCamera* PlayerCharacter)
{
	auto Enemy = PlayerCharacter->GetCurrentUnit();
	if (GridManager && Enemy)
	{
		// Get the current position of the player and the enemy
		FVector2D PlayerPosition = this->CurrentGridPosition;
		FVector2D EnemyPosition = Enemy->CurrentGridPosition;

		// Define attack range
		float AttackRange = 2.0f; // Example value, adjust as needed
		float DistanceToEnemy = FVector2D::Distance(PlayerPosition, EnemyPosition);

		// Check if the player is within attack range
		if (DistanceToEnemy > AttackRange)
		{
			UE_LOG(LogTemp, Warning, TEXT("Player is out of range to attack the enemy."));
			return;
		}

		// Retrieve all neighboring tiles around the enemy
		TArray<AGridTile*> NeighbourTiles = GridManager->GetNeighbourTiles(EnemyPosition.X, EnemyPosition.Y);

		AGridTile* BestTile = nullptr;
		float ClosestDistanceToEnemy = FLT_MAX;

		// Check if there are valid, unoccupied tiles around the enemy
		for (AGridTile* Tile : NeighbourTiles)
		{
			if (Tile && !Tile->bIsOccupied)
			{
				// Calculate distance to the enemy from this tile
				float TileDistanceToEnemy = FVector2D::Distance(Tile->GridPosition, EnemyPosition);

				// Prioritize the closest tile
				if (TileDistanceToEnemy < ClosestDistanceToEnemy && TileDistanceToEnemy < movementSpeed)
				{
					ClosestDistanceToEnemy = TileDistanceToEnemy;
					BestTile = Tile;
				}
			}
		}

		if (BestTile && ClosestDistanceToEnemy < movementSpeed)
		{
			UE_LOG(LogTemp, Log, TEXT("Player moving to tile (%f, %f) to attack the enemy"), BestTile->GridPosition.X, BestTile->GridPosition.Y);
			this->MoveToTile(BestTile->GridPosition);

			// Only attack if the player has reached the correct tile
			if (this->CurrentGridPosition == BestTile->GridPosition)
			{
				combatManager->DealDamageToUnit(Enemy, this);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No valid tile found or player is out of movement range."));
		}
	}
}



void APlayerUnit::ExecutePlayerTurn()
{
	//some logic here
	UE_LOG(LogTemp, Error, TEXT("Player did something"));
	//FinishTurn();
}

void APlayerUnit::ExecuteAITurn()
{
	//Some AI logic here
	UE_LOG(LogTemp, Error, TEXT("AI did something"));
	this->MoveToGridPosition();
	FinishTurn();
}

void APlayerUnit::FinishTurn()
{
	AGameManager* GameManager = Cast<AGameManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameManager::StaticClass()));
	if (GameManager)
	{
		GameManager->EndUnitTurn();
	}
}

void APlayerUnit::DestoryUnit()
{
	if (GridManager)
	{
		AActor* Tile = GridManager->GetTileAt(CurrentGridPosition.X, CurrentGridPosition.Y);
		AGridTile* GridTile = Cast<AGridTile>(Tile);
		if (GridTile)
		{
			GridTile->RemoveUnitRefrence();
			GridTile->bIsOccupied = false;
		}
	}

	
}

void APlayerUnit::BeginFocus()
{
	if (mesh)
	{
		mesh->SetRenderCustomDepth(true);
	}
}

void APlayerUnit::EndFocus()
{
	if (mesh)
	{
		mesh->SetRenderCustomDepth(false);
	}
}

void APlayerUnit::Interact(APlayerCamera* PlayerCharacter)
{
	if (PlayerCharacter)
	{
	
		UpdateInteractableData();

		if(!this->bIsPlayerUnit)
		{
			PlayerAttack(PlayerCharacter);
		}
		
	}
}



void APlayerUnit::UpdateInteractableData()
{
	InstanceInteractableData.InteractableType = EInteractionType::IT_Interact;

	//Add more data here
}


//AI stuff (Should be moved to a AI controller)
void APlayerUnit::MoveToGridPosition()
{
		auto closestEnemy = this->FindEnemyunit();

		if (IsValid(closestEnemy))
		{
			float Distance = FVector2D::Distance(CurrentGridPosition, closestEnemy->CurrentGridPosition);

			if (Distance < movementSpeed)
			{
				this->Attack(closestEnemy);
			}
			else
			{
				this->MoveToClosestPossibleTile(closestEnemy);
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("No valid enemy found"));
		}
}

APlayerUnit* APlayerUnit::FindEnemyunit()
{
	FVector2D closestUnitPos = FVector2D::ZeroVector;
	APlayerUnit* closestUnit = nullptr;

	if (!GridManager)
	{
		UE_LOG(LogTemp, Error, TEXT("GridManager is not valid or has not been possessed"));
		return nullptr;
	}

	int numTilesChecked = 0;

	// Iterate over the TMap of GridTiles
	for (const auto& TilePair : GridManager->GridTiles)
	{
		// Cast the AActor* value to AGridTile*
		AGridTile* Tile = Cast<AGridTile>(TilePair.Value);
		if (!Tile)
		{
			UE_LOG(LogTemp, Warning, TEXT("Invalid tile found in GridTiles"));
			continue;
		}

		// Access the unit reference
		APlayerUnit* Unit = Tile->unitRefrence;
		if (!Unit)
		{
			UE_LOG(LogTemp, Verbose, TEXT("Tile at position %s has no unit reference"), *TilePair.Key.ToString());
			continue;
		}

		// Skip self-reference
		if (Unit == this)
		{
			UE_LOG(LogTemp, Verbose, TEXT("Skipping self"));
			continue;
		}

		// Check if the unit is a "friendly" (same bIsPlayerUnit value as this AI)
		if (Unit->bIsPlayerUnit != this->bIsPlayerUnit)
		{
			UE_LOG(LogTemp, Verbose, TEXT("Skipping enemy unit: %s"), *Unit->GetName());
			//continue;
		}

		// Find the closest friendly unit
		if (!closestUnit || FVector2D::Distance(this->CurrentGridPosition, Unit->CurrentGridPosition) < FVector2D::Distance(this->CurrentGridPosition, closestUnitPos))
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


void APlayerUnit::MoveToClosestPossibleTile(APlayerUnit* Enemy)
{
	if (GridManager && Enemy)
	{
		// Get the current grid position of the AI unit and the enemy
		FVector2D CurrentPosition = this->CurrentGridPosition;
		FVector2D EnemyPosition = Enemy->CurrentGridPosition;

		TArray<AGridTile*> PossibleTiles;
		PossibleTiles = GridManager->GetNeighbourTiles(this->CurrentGridPosition.X, this->CurrentGridPosition.Y);

		AGridTile* BestTile = nullptr;
		float BestScore = FLT_MAX; // Lower scores are better

		// Iterate through all possible tiles
		for (AGridTile* Tile : PossibleTiles)
		{
			// Calculate distance from the current position to the tile
			float DistanceToTile = FVector2D::Distance(CurrentPosition, Tile->GridPosition);

			// Only consider tiles within movement range
			if (DistanceToTile <= this->movementSpeed)
			{
				// Calculate the total score: distance to the tile + distance from tile to the enemy
				float DistanceToEnemyFromTile = FVector2D::Distance(Tile->GridPosition, EnemyPosition);
				float TotalScore = DistanceToTile + DistanceToEnemyFromTile;

				// Update the best tile if this score is better
				if (TotalScore < BestScore)
				{
					BestScore = TotalScore;
					BestTile = Tile;
				}
			}
		}

		if (BestTile && !bIsPlayerUnit)
		{
			this->MoveToTile(BestTile->GridPosition);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No valid tile found within range"));
			// Handle the case where no valid tile is found (e.g., stay put)
			FinishTurn();
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GridManager or Enemy is not valid"));
		FinishTurn();
	}
}


void APlayerUnit::Attack(APlayerUnit* Enemy)
{
	if (GridManager && !bIsPlayerUnit && Enemy)
	{
		// Get the current grid position of the enemy and AI
		FVector2D AIPosition = this->CurrentGridPosition;
		FVector2D EnemyPosition = Enemy->CurrentGridPosition;

		// Retrieve all neighboring tiles around the enemy (up, down, left, right, and diagonals)
		TArray<AGridTile*> NeighbourTiles = GridManager->GetNeighbourTiles(EnemyPosition.X, EnemyPosition.Y);

		AGridTile* BestTile = nullptr;
		float ClosestDistanceToEnemy = FLT_MAX;

		// Check if there are unoccupied and valid tiles around the enemy
		for (AGridTile* Tile : NeighbourTiles)
		{
			if (Tile && !Tile->bIsOccupied)
			{
				// Calculate the distance to the enemy from this tile
				float DistanceToEnemy = FVector2D::Distance(Tile->GridPosition, EnemyPosition);

				// Prioritize the closest tile to the enemy
				if (DistanceToEnemy < ClosestDistanceToEnemy)
				{
					ClosestDistanceToEnemy = DistanceToEnemy;
					BestTile = Tile;
				}
			}
		}

		if (BestTile)
		{
			// Move to the selected tile and perform the attack logic
			UE_LOG(LogTemp, Log, TEXT("AI moving to tile (%f, %f) to attack the enemy"), BestTile->GridPosition.X, BestTile->GridPosition.Y);
			this->MoveToTile(BestTile->GridPosition);
			// Call the attack logic (ensure attack can happen here)
			combatManager->DealDamageToUnit(this, Enemy);
		}
		else
		{
			// If no valid tile is found directly adjacent, attempt to move closer to the enemy (even if 2 tiles away)
			UE_LOG(LogTemp, Warning, TEXT("No valid tiles directly adjacent to the enemy. Moving closer instead."));

			// You can implement a range check here to move to the nearest available tile towards the enemy
			MoveToClosestPossibleTile(Enemy);
		}
	}
	else
	{
		if (!GridManager) UE_LOG(LogTemp, Error, TEXT("GridManager is invalid"));
		if (!Enemy) UE_LOG(LogTemp, Error, TEXT("Enemy is null"));
		if (bIsPlayerUnit) UE_LOG(LogTemp, Warning, TEXT("Player-controlled units cannot use this AI logic"));
	}


}

