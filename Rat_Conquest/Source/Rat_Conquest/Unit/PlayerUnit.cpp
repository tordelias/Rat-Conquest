// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerUnit.h"

//Engine

//includes
#include "Rat_Conquest/GridManager/GridManager.h"
#include "Rat_Conquest/Player/PlayerCamera.h"
#include "Rat_Conquest/GameManager/GameManager.h"
#include "Rat_Conquest/GridTile/GridTile.h"
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
			this->MoveToGridPsoition();
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


void APlayerUnit::ExecutePlayerTurn()
{
	//some logic here
	UE_LOG(LogTemp, Error, TEXT("Player did something"));
	FinishTurn();
}

void APlayerUnit::ExecuteAITurn()
{
	//Some AI logic here
	UE_LOG(LogTemp, Error, TEXT("AI did something"));
	this->MoveToGridPsoition();
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
		// hopefully removed at a later stage 
		PlayerCharacter->SetCurrentUnit(this);
		UpdateInteractableData();
	}
}



void APlayerUnit::UpdateInteractableData()
{
	InstanceInteractableData.InteractableType = EInteractionType::IT_Interact;

	//Add more data here
}


//AI stuff (Should be moved to a AI controller)
void APlayerUnit::MoveToGridPsoition()
{
		auto closestEnemy = this->FindEnemyunit();

		if (IsValid(closestEnemy))
		{
			float Distance = FVector2D::Distance(CurrentGridPosition, closestEnemy->CurrentGridPosition);

			if (Distance <= movementSpeed)
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
			//continue;
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
	if (GridManager)
	{
		// Get the current grid position of the AI unit
		FVector CurrentPosition = this->GetActorLocation();
		TArray<AGridTile*> PossibleTiles;

		// Get all the neighboring tiles or nearby tiles
		PossibleTiles = GridManager->GetNeighbourTiles(this->CurrentGridPosition.X, this->CurrentGridPosition.Y);

		AGridTile* ClosestTile = nullptr;
		float ClosestDistance = FLT_MAX; // Start with an infinitely large distance

		// Iterate through all the possible tiles
		for (AGridTile* Tile : PossibleTiles)
		{
			// Calculate the distance from the AI's current position to this tile
			float DistanceToTile = FVector::Dist(CurrentPosition, Tile->GetActorLocation());

			// Check if this tile is within the AI unit's movement range
			if (DistanceToTile <= this->movementSpeed)
			{
				// Check if it's the closest tile so far
				if (DistanceToTile < ClosestDistance)
				{
					ClosestDistance = DistanceToTile;
					ClosestTile = Tile;
				}
			}
		}

		if (ClosestTile && !bIsPlayerUnit)
		{
			this->MoveToTile(ClosestTile->GridPosition);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No valid tile found within range"));
			// If no valid tile is found within range, the AI cannot move
			// Optionally, handle this case (e.g., stay put, move randomly, etc.)
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GridManager is not valid or has not been possessed"));
	}
}

void APlayerUnit::Attack(APlayerUnit* Enemy)
{
	if (GridManager && !bIsPlayerUnit)
	{
		TArray<AGridTile*> NeighbourTiles = GridManager->GetNeighbourTiles(Enemy->CurrentGridPosition.X, Enemy->CurrentGridPosition.Y);
		for (auto& Tile : NeighbourTiles)
		{
			if (Tile->bIsOccupied == false)
			{
				this->MoveToTile(Tile->GridPosition);
				//Atack code here
				return;
			}
		}
	}
}

