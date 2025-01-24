// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerUnit.h"

//Engine

//includes
#include "Rat_Conquest/Managers/GridManager/GridManager.h"
#include "Rat_Conquest/Player/PlayerCamera.h"
#include "Rat_Conquest/Managers/GameManager/GameManager.h"
#include "Rat_Conquest/GridTile/GridTile.h"
#include "Rat_Conquest/Managers/CombatManager/CombatManager.h"
#include "Rat_Conquest/Items/Item.h"
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
			SetActorLocation(FVector(TargetPosition.X,TargetPosition.Y,GetActorLocation().Z));
			UE_LOG(LogTemp, Display, TEXT("Finished moving to new tile: %s"), *TargetPosition.ToString());
		}
		else
		{
			FVector NewPosition = FMath::Lerp(StartPosition, TargetPosition, MovementProgress);
			SetActorLocation(FVector(NewPosition.X, NewPosition.Y, GetActorLocation().Z));

			FVector Direction = (FVector(TargetPosition.X, TargetPosition.Y, 0) - FVector(GetActorLocation().X, GetActorLocation().Y, 0)).GetSafeNormal(); // Get normalized direction vector
			FRotator TargetRotation = Direction.Rotation(); // Convert direction to rotation
			FRotator CurrentRotation = GetActorRotation();

			// Ensure the mesh aligns properly with the forward X-axis
			TargetRotation.Yaw += 90.0f * -1; // Add a 90-degree offset to align X as forward, if necessary

			// Constrain rotation to yaw only
			TargetRotation.Pitch = CurrentRotation.Pitch;
			TargetRotation.Roll = CurrentRotation.Roll;

			// Smoothly interpolate to the target yaw rotation
			FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, 5.0f);
			SetActorRotation(NewRotation);
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
	CheckForItems();
	if (bIsPlayerUnit)
	{
		FinishTurn();
	}
}

void APlayerUnit::SetInitalPosition(FVector2D position)
{
	if (!GridManager) {
		UE_LOG(LogTemp, Error, TEXT("NO MANAGER"));
		return;
	}

	AActor* TargetTile = GridManager->GetClosestAvailableTile(position);
	if (!TargetTile) {
		UE_LOG(LogTemp, Warning, TEXT("Invalid target tile at (%f, %f)"), position.X, position.Y);
		return;
	}

	TargetPosition = TargetTile->GetActorLocation();
	SetActorLocation(FVector(TargetPosition.X, TargetPosition.Y, GetActorLocation().Z));
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

		// Define attack range, which is based on movement range
		float AttackRange = movementSpeed; // Player's movement range
		float DistanceToEnemy = FVector2D::Distance(PlayerPosition, EnemyPosition);

		// Check if the enemy is within movement range of the player
		if (DistanceToEnemy > AttackRange)
		{
			UE_LOG(LogTemp, Warning, TEXT("Player is out of range to move to the enemy's neighboring tiles."));
			return; // Player can't attack if they can't move to a neighboring tile
		}

		// Retrieve all neighboring tiles around the enemy
		TArray<AGridTile*> NeighbourTiles = GridManager->GetNeighbourTiles(EnemyPosition.X, EnemyPosition.Y);

		AGridTile* BestTile = nullptr;
		float ClosestDistanceToEnemy = FLT_MAX;

		// Find the closest valid, unoccupied neighboring tile around the enemy
		for (AGridTile* Tile : NeighbourTiles)
		{
			if (Tile && !Tile->bIsOccupied)
			{
				// Calculate distance to the player from this tile
				float TileDistanceToPlayer = FVector2D::Distance(Tile->GridPosition, PlayerPosition);

				// Prioritize the closest tile within the movement range
				if (TileDistanceToPlayer < ClosestDistanceToEnemy && TileDistanceToPlayer <= movementSpeed)
				{
					ClosestDistanceToEnemy = TileDistanceToPlayer;
					BestTile = Tile;
				}
			}
		}

		if (BestTile)
		{
			// Move the player to the closest neighboring tile around the enemy
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
			UE_LOG(LogTemp, Warning, TEXT("No valid neighboring tile found within movement range."));
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
	MoveToGridPosition();
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
			UE_LOG(LogTemp, Error, TEXT("AHH YOU KILLED ME!! from grid tile at (%f, %f)"), CurrentGridPosition.X, CurrentGridPosition.Y);
			
		}
	}

	
}

void APlayerUnit::CheckForItems()
{
	if (!GridManager) {
		UE_LOG(LogTemp, Error, TEXT("NO MANAGER"));
		return;
	}

	AActor* TargetTile = GridManager->GetTileAt(CurrentGridPosition.X, CurrentGridPosition.Y);

	if (!TargetTile)
	{
		UE_LOG(LogTemp, Warning, TEXT("No item at tile (%f, %f)"), CurrentGridPosition.X, CurrentGridPosition.Y);
		return;
	}
	AGridTile* tile = Cast<AGridTile>(TargetTile);
	if (tile) {
		if (tile->itemSlot) {

			AItem* item = tile->itemSlot;
			WeaponSlot = item;
			item->EquipItem();
			tile->itemSlot = nullptr;

			
			UE_LOG(LogTemp, Error, TEXT("Player picked up item at tile (%f, %f)"), CurrentGridPosition.X, CurrentGridPosition.Y);	

		}

	}

}

void APlayerUnit::CalculateStats()
{

}

void APlayerUnit::BeginFocus()
{
	if (mesh)
	{
		mesh->SetRenderCustomDepth(true);
	}
	auto movableTiles = GridManager->GetMovableTiles(CurrentGridPosition.X, CurrentGridPosition.Y, movementSpeed);
	for (auto tile : movableTiles)
	{
		tile->BeginFocus();
	}
	MovedTiles = movableTiles;
}

void APlayerUnit::EndFocus()
{
	if (mesh)
	{
		mesh->SetRenderCustomDepth(false);
	}
	for (auto tile : MovedTiles)
	{
		tile->EndFocus();
	}
}

void APlayerUnit::BeginMouseHoverFocus()
{
	// Get tiles within movement range
	if (GridManager)
	{
		TArray<AGridTile*> HoverableTiles = GridManager->GetMovableTiles(CurrentGridPosition.X, CurrentGridPosition.Y, movementSpeed);
		for (AGridTile* Tile : HoverableTiles)
		{
			if (Tile)
			{
				Tile->BeginMouseHoverFocus();
			}
		}
		MovedTiles = HoverableTiles; 
	}
}

void APlayerUnit::EndMouseHoverFocus()
{
	for (AGridTile* Tile : MovedTiles)
	{
		if (Tile)
		{
			Tile->EndMouseHoverFocus(); 
		}
	}
	MovedTiles.Empty();
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

	if (bIsPlayerUnit)
	{
		InstanceInteractableData.UnitName = FText::FromString("SwordsMan");
	}
	else
	{
		InstanceInteractableData.UnitName = FText::FromString("Rat");
	}

	InstanceInteractableData.UnitHealth = health;
	InstanceInteractableData.UnitDamage = damage;
	InstanceInteractableData.UnitMovementSpeed = movementSpeed;

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

	for (const auto& TilePair : GridManager->GridTiles)
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

		if (Unit == this)
		{
			UE_LOG(LogTemp, Verbose, TEXT("Skipping self"));
			continue;
		}

		if (!Unit->bIsPlayerUnit)
		{
			UE_LOG(LogTemp, Verbose, TEXT("Skipping enemy unit: %s"), *Unit->GetName());
			continue;
			
		}

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
		FVector2D CurrentPosition = this->CurrentGridPosition;
		FVector2D EnemyPosition = Enemy->CurrentGridPosition;

		TArray<AGridTile*> PossibleTiles;
		PossibleTiles = GridManager->GetNeighbourTiles(this->CurrentGridPosition.X, this->CurrentGridPosition.Y);

		AGridTile* BestTile = nullptr;
		float BestScore = FLT_MAX; 

		for (AGridTile* Tile : PossibleTiles)
		{
			float DistanceToTile = FVector2D::Distance(CurrentPosition, Tile->GridPosition);

			if (DistanceToTile <= this->movementSpeed)
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

		if (BestTile && !this->bIsPlayerUnit)
		{
			this->MoveToTile(BestTile->GridPosition);
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


void APlayerUnit::Attack(APlayerUnit* Enemy)
{
	if (GridManager && !this->bIsPlayerUnit && Enemy)
	{
		FVector2D AIPosition = this->CurrentGridPosition;
		FVector2D EnemyPosition = Enemy->CurrentGridPosition;

		TArray<AGridTile*> NeighbourTiles = GridManager->GetNeighbourTiles(EnemyPosition.X, EnemyPosition.Y);

		AGridTile* BestTile = nullptr;
		float ClosestDistanceToEnemy = FLT_MAX;

		for (AGridTile* Tile : NeighbourTiles)
		{
			if (Tile && !Tile->bIsOccupied)
			{
				float DistanceToEnemy = FVector2D::Distance(Tile->GridPosition, EnemyPosition);

				if (DistanceToEnemy < ClosestDistanceToEnemy)
				{
					ClosestDistanceToEnemy = DistanceToEnemy;
					BestTile = Tile;
				}
			}
		}

		if (BestTile && Enemy->bIsPlayerUnit)
		{
			UE_LOG(LogTemp, Log, TEXT("AI moving to tile (%f, %f) to attack the enemy"), BestTile->GridPosition.X, BestTile->GridPosition.Y);
			this->MoveToTile(BestTile->GridPosition);
			combatManager->DealDamageToUnit(this, Enemy);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No valid tiles directly adjacent to the enemy. Moving closer instead."));
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

