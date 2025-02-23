// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerUnit.h"

//Engine
#include "Kismet/GameplayStatics.h" // Include for UGameplayStatics
#include "Components/CapsuleComponent.h" // Include for UCapsuleComponent
//includes
#include "Rat_Conquest/Managers/GridManager/GridManager.h"
#include "Rat_Conquest/Player/PlayerCamera.h"
#include "Rat_Conquest/Managers/GameManager/GameManager.h"
#include "Rat_Conquest/GridTile/GridTile.h"
#include "Rat_Conquest/Managers/CombatManager/CombatManager.h"
#include "Rat_Conquest/Items/Item.h"
#include "Rat_Conquest/AI/EnemyAIController.h"
#include "Rat_Conquest/Projectiles/GenericProjectile.h"



APlayerUnit::APlayerUnit()
{
	// Set this pawn to call Tick() every frame.
	PrimaryActorTick.bCanEverTick = true;

	UE_LOG(LogTemp, Warning, TEXT("APlayerUnit Constructor - Start"));

	// ? Create CapsuleComponent FIRST before assigning it to RootComponent
	UCapsuleComponent* CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CapsuleComp->InitCapsuleSize(42.f, 96.0f);
	CapsuleComp->SetCollisionProfileName(TEXT("Pawn"));

	// ? Assign it as the RootComponent
	RootComponent = CapsuleComp;

	UE_LOG(LogTemp, Warning, TEXT("CapsuleComponent Created and Assigned as RootComponent"));

	// ? Now create the SkeletalMeshComponent
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(RootComponent); // Attach properly

	SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SkeletalMesh->SetCollisionObjectType(ECC_Pawn);
	SkeletalMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	SkeletalMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	SkeletalMesh->SetGenerateOverlapEvents(true);

	UE_LOG(LogTemp, Warning, TEXT("SkeletalMesh Created and Collision Set"));

	// Initialize other properties
	ItemSlots.SetNum(3);
	GridStartPosition = FVector2D(0, 0);
	GridManager = nullptr;
	combatManager = nullptr;
	bIsRangedUnit = false;

	UE_LOG(LogTemp, Warning, TEXT("APlayerUnit Constructor - End"));
}


// Called when the game starts or when spawned
void APlayerUnit::BeginPlay()
{
	Super::BeginPlay();

	//if (bIsPlayerUnit) {
	//	DelayedInitalPosition();
	//}
	TArray<AActor*> FoundGridManagers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGridManager::StaticClass(), FoundGridManagers);

	if (FoundGridManagers.Num() > 0)
	{
		GridManager = Cast<AGridManager>(FoundGridManagers[0]);
		if (FoundGridManagers.Num() > 1)
		{
			UE_LOG(LogTemp, Warning, TEXT("Multiple GridManagers found! Using first instance."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No GridManager found in the level!"));
	}

	TArray<AActor*> FoundCombatManagers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACombatManager::StaticClass(), FoundCombatManagers);

	if (FoundCombatManagers.Num() > 0)
	{
		combatManager = Cast<ACombatManager>(FoundCombatManagers[0]);
		if (FoundCombatManagers.Num() > 1)
		{
			UE_LOG(LogTemp, Warning, TEXT("Multiple Combatmanagers found! Using first instance."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No CombatManager found in the level!"));
	}

		AGameManager* GameManager = Cast<AGameManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameManager::StaticClass()));

		if (!GameManager)
		{
			UE_LOG(LogTemp, Error, TEXT("BeginPlay(): No GameManager found!"));
		}

	DelayedInitalPosition();
	this->UpdateInteractableData();

	if (bIsPlayerUnit)
	{
		SkeletalMesh->SetCustomDepthStencilValue(1);
	}
	else
	{
		SkeletalMesh->SetCustomDepthStencilValue(2);
	}
	if (StartWeapon)
	{
		//spawn from bp
		AItem* StartWeaponItem = GetWorld()->SpawnActor<AItem>(StartWeapon, GetActorLocation(), FRotator::ZeroRotator);
		EquipStartWeapon(StartWeaponItem);
	}
}

// Called every frame
void APlayerUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsMoving)
	{
		MovementProgress += DeltaTime / MovementDuration;
		bIsCurrentUnit = false;
		if (MovementProgress >= 1.0f)
		{
			MovementProgress = 1.0f;
			bIsMoving = false;
			
			// Update the unit's position to the target tile
			SetActorLocation(FVector(TargetPosition.X, TargetPosition.Y, GetActorLocation().Z));
			UE_LOG(LogTemp, Display, TEXT("Finished moving to tile: %s"), *TargetPosition.ToString());

			// Update the current grid position
			CurrentGridPosition = TargetGridPosition;

			// Check if there are more tiles in the path
			if (PathToTake.Num() > 0)
			{
				// Pop the next tile from the path
				FVector2D NextTilePosition = PathToTake[0];
				PathToTake.RemoveAt(0);

				// Log the updated path for debugging
				UE_LOG(LogTemp, Display, TEXT("Updated PathToTake:"));
				for (FVector2D TilePosition : PathToTake)
				{
					UE_LOG(LogTemp, Display, TEXT("- %s"), *TilePosition.ToString());
				}

				// Set up movement to the next tile
				AActor* NextTile = GridManager->GetTileAt(NextTilePosition.X, NextTilePosition.Y);
				if (NextTile)
				{
					StartPosition = GetActorLocation();
					TargetPosition = NextTile->GetActorLocation();
					TargetGridPosition = NextTilePosition;
					MovementProgress = 0.0f; // Reset progress
					bIsMoving = true;        // Start movement
				}
			}
			else
			{
				// Path is complete
				UE_LOG(LogTemp, Display, TEXT("Finished moving along the path."));

				// Trigger the OnMovementComplete delegate if it's bound
				if (OnMovementComplete.IsBound())
				{
					OnMovementComplete.Execute();
					OnMovementComplete.Unbind(); // Unbind the delegate after execution
				}

				// Other logic (e.g., checking for items, finishing the turn)
				
				if (bIsPlayerUnit)
				{
					FinishTurn();
				}
			}
		}
		else
		{
			// Interpolate position between StartPosition and TargetPosition
			FVector NewPosition = FMath::Lerp(StartPosition, TargetPosition, MovementProgress);
			SetActorLocation(FVector(NewPosition.X, NewPosition.Y, GetActorLocation().Z));

			// Rotate the unit to face the direction of movement
			FVector Direction = (TargetPosition - StartPosition).GetSafeNormal();
			FRotator TargetRotation = Direction.Rotation();
			FRotator CurrentRotation = GetActorRotation();

			// Adjust rotation to align with the unit's forward direction
			TargetRotation.Yaw += 90.0f * -1;

			// Constrain rotation to yaw only
			TargetRotation.Pitch = CurrentRotation.Pitch;
			TargetRotation.Roll = CurrentRotation.Roll;

			// Smoothly interpolate to the target yaw rotation
			FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, 5.0f);
			SetActorRotation(NewRotation);
		}
	}

	//temp
	if (bIsKnockbackActive)
	{
		// Update knockback progress
		KnockbackProgress += DeltaTime / KnockbackDuration;

		if (KnockbackProgress >= 1.0f)
		{
			// Snap to the target position when the knockback is complete
			SetActorLocation(KnockbackTargetPosition);
			bIsKnockbackActive = false;
		}
		else
		{
			// Interpolate between the start and target positions
			FVector NewPosition = FMath::Lerp(KnockbackStartPosition, KnockbackTargetPosition, KnockbackProgress);
			SetActorLocation(NewPosition);
		}
	}


}

FVector2D APlayerUnit::GetMousePosition(FVector WorldLocation, FVector WorldDirection)
{
	if (!UGameplayStatics::GetPlayerController(GetWorld(), 0)->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
		return FVector2D();

	FVector MouseWorldPos = FMath::LinePlaneIntersection(
		WorldLocation,
		WorldLocation + WorldDirection * 10000.f,
		FPlane(FVector(0, 0, GridManager->GridHeight),
			FVector::UpVector
		));

	UE_LOG(LogTemp, Display, TEXT("MouseWorldPos: %s"), *MouseWorldPos.ToString());
	MouseWorldPos.Z = GridManager->GridHeight;
	return FVector2D(MouseWorldPos.X, MouseWorldPos.Y);
}


void APlayerUnit::MoveToTile(FVector2D NewGridPosition)
{
	if (!bIsCurrentUnit && bIsPlayerUnit)
		return;
	if (!GridManager)
	{
		UE_LOG(LogTemp, Error, TEXT("NO MANAGER |MoveToTile_PlayerUnit.cpp|"));
		return;
	}

	AActor* TargetTile = GridManager->GetTileAt(NewGridPosition.X, NewGridPosition.Y);
	if (!TargetTile)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid target tile at (%f, %f) |MoveToTile_PlayerUnit.cpp|"), NewGridPosition.X, NewGridPosition.Y);
		return;
	}

	AGridTile* GridTile = Cast<AGridTile>(TargetTile);
	if (!GridTile)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to cast TargetTile to AGridTile |MoveToTile_PlayerUnit.cpp|"));
		return;
	}

	if (GridTile->bIsOccupied)
	{
		UE_LOG(LogTemp, Warning, TEXT("Target tile is occupied |MoveToTile_PlayerUnit.cpp|"));
		return;
	}

	// Calculate the path
	if (!CalculatePathToTile(NewGridPosition))
	{
		UE_LOG(LogTemp, Warning, TEXT("No valid path found to target tile |MoveToTile_PlayerUnit.cpp|"));
		return;
	}

	// Free the current tile
	AActor* OldTile = GridManager->GetTileAt(CurrentGridPosition.X, CurrentGridPosition.Y);
	AGridTile* OldGridTile = Cast<AGridTile>(OldTile);
	if (!OldGridTile)
	{
		UE_LOG(LogTemp, Error, TEXT("OldGridTile is null |MoveToTile_PlayerUnit.cpp|"));
		return;
	}

	OldGridTile->bIsOccupied = false;
	OldGridTile->RemoveUnitRefrence();

	// Check if the target tile is within movement range
	float distance = ChebyshevDistance(GridTile->GridPosition, OldGridTile->GridPosition);
	if (distance > MovementSpeed)
	{
		UE_LOG(LogTemp, Warning, TEXT("Target tile is out of movement range |MoveToTile_PlayerUnit.cpp|"));
		return;
	}

	// Log the path for debugging
	UE_LOG(LogTemp, Display, TEXT("PathToTake:"));
	for (FVector2D TilePosition : PathToTake)
	{
		UE_LOG(LogTemp, Display, TEXT("- %s"), *TilePosition.ToString());
	}

	// Mark the new tile as occupied
	GridTile->bIsOccupied = true;
	GridTile->SetUnitRefrence(this);

	// Set up movement to the first tile in the path
	if (PathToTake.Num() > 0)
	{
		FVector2D NextTilePosition = PathToTake[0];
		PathToTake.RemoveAt(0);

		AActor* NextTile = GridManager->GetTileAt(NextTilePosition.X, NextTilePosition.Y);
		if (NextTile)
		{
			StartPosition = GetActorLocation();
			TargetPosition = NextTile->GetActorLocation();
			TargetGridPosition = NextTilePosition;
			MovementProgress = 0.0f; // Reset progress
			bIsMoving = true;        // Start movement
		}
	}
}

bool APlayerUnit::CalculatePathToTile(FVector2D InTargetGridPosition)
{
	if (!GridManager)
	{
		UE_LOG(LogTemp, Error, TEXT("NO MANAGER"));
		return false;
	}

	PathToTake = GetPathToTile(InTargetGridPosition, CurrentGridPosition);
	return PathToTake.Num() > 0;
}

TArray<FVector2D> APlayerUnit::GetPathToTile(FVector2D InTargetGridPosition, FVector2D StartTile)
{
	GridManager->ResetAllTilesPathfindingData();
	TArray<FVector2D> Path;
	TArray<AGridTile*> OpenList;
	TArray<AGridTile*> ClosedList;
	TMap<AGridTile*, AGridTile*> CameFrom;

	AGridTile* StartTilePtr = Cast<AGridTile>(GridManager->GetTileAt(StartTile.X, StartTile.Y));
	AGridTile* TargetTilePtr = Cast<AGridTile>(GridManager->GetTileAt(InTargetGridPosition.X, InTargetGridPosition.Y));

	if (!StartTilePtr || !TargetTilePtr)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid start or target tile"));
		return TArray<FVector2D>();
	}
	/*if (TargetTilePtr->bIsOccupied)
	{
		UE_LOG(LogTemp, Warning, TEXT("Target tile is occupied"));
		return TArray<FVector2D>();
	}*/
	// Initialize the open list with the start tile
	OpenList.Add(StartTilePtr);
	StartTilePtr->G = 0;
	StartTilePtr->H = ChebyshevDistance(StartTilePtr->GridPosition, InTargetGridPosition);
	StartTilePtr->F = StartTilePtr->G + StartTilePtr->H;

	while (OpenList.Num() > 0)
	{
		// Find the tile with the lowest F cost in the open list
		AGridTile* CurrentTile = OpenList[0];
		for (AGridTile* Tile : OpenList)
		{
			if (Tile->F < CurrentTile->F)
			{
				CurrentTile = Tile;
			}
		}

		// If we've reached the target tile, reconstruct the path
		if (CurrentTile->GridPosition == InTargetGridPosition)
		{
			AGridTile* PathTile = CurrentTile;
			while (PathTile != StartTilePtr)
			{
				Path.Add(PathTile->GridPosition);
				PathTile = CameFrom[PathTile];
			}
			Path.Add(StartTilePtr->GridPosition);
			Algo::Reverse(Path); // Reverse the path to get it from start to target

			// Log the path for debugging
			UE_LOG(LogTemp, Display, TEXT("Calculated Path:"));
			for (FVector2D TilePosition : Path)
			{
				UE_LOG(LogTemp, Display, TEXT("- %s"), *TilePosition.ToString());
			}

			return Path;
		}

		// Move the current tile from the open list to the closed list
		OpenList.Remove(CurrentTile);
		ClosedList.Add(CurrentTile);

		// Get the neighboring tiles
		TArray<AGridTile*> NeighbourTiles = GridManager->GetNeighbourTiles(CurrentTile->GridPosition.X, CurrentTile->GridPosition.Y);

		for (AGridTile* Neighbour : NeighbourTiles)
		{
			if (!Neighbour || Neighbour->bIsOccupied || ClosedList.Contains(Neighbour))
			{
				continue; // Skip occupied or already evaluated tiles
			}

			// Calculate the tentative G cost
			float TentativeG = CurrentTile->G + ChebyshevDistance(CurrentTile->GridPosition, Neighbour->GridPosition);

			if (!OpenList.Contains(Neighbour) || TentativeG < Neighbour->G)
			{
				// This path to the neighbor is better than any previous one
				CameFrom.Add(Neighbour, CurrentTile);
				Neighbour->G = TentativeG;
				Neighbour->H = ChebyshevDistance(Neighbour->GridPosition, InTargetGridPosition);
				Neighbour->F = Neighbour->G + Neighbour->H;

				if (!OpenList.Contains(Neighbour))
				{
					OpenList.Add(Neighbour);
				}
			}
		}
	}

	// If the loop ends without finding the target, return an empty path
	UE_LOG(LogTemp, Warning, TEXT("No path found to target tile"));
	return TArray<FVector2D>();
}

void APlayerUnit::SetInitalPosition(FVector2D position)
{
	if (!GridManager) {
		UE_LOG(LogTemp, Error, TEXT("NO MANAGER |SetInitalPosition_PlayerUnit.cpp|"));
		return;
	}

	AActor* TargetTile = GridManager->GetClosestAvailableTile(position);
	if (!TargetTile) {
		UE_LOG(LogTemp, Warning, TEXT("Invalid target tile at (%f, %f) |SetInitalPosition_PlayerUnit.cpp|"), position.X, position.Y);
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
	if (!this->bIsPlayerUnit)
	{
		UE_LOG(LogTemp, Display, TEXT("AI start pos was set"));
	}
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
float APlayerUnit::ChebyshevDistance(FVector2D A, FVector2D B)
{
	return FMath::Max(FMath::Abs(A.X - B.X), FMath::Abs(A.Y - B.Y));
}

void APlayerUnit::PlayerAttack(APlayerCamera* PlayerCharacter)
{
	if (!PlayerCharacter || !GridManager)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerCharacter or GridManager is null"));
		return;
	}

	auto PlayerUnit = PlayerCharacter->GetCurrentUnit();
	if (!PlayerUnit)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerUnit is null"));
		return;
	}

	FVector2D PlayerPosition = PlayerUnit->CurrentGridPosition;
	FVector2D EnemyPosition = this->CurrentGridPosition;
	TargetEnemyLocation = EnemyPosition;

	float DistanceToEnemy = ChebyshevDistance(EnemyPosition, PlayerPosition);

	if (PlayerUnit->bIsRangedUnit)
	{
		if (DistanceToEnemy <= AttackRange)
		{
			UE_LOG(LogTemp, Log, TEXT("Ranged unit attacking from a distance"));
			PlayerUnit->ShootProjectile(this->GetActorLocation());
			combatManager->DealDamageToUnit(PlayerUnit, this);
			PlayerCharacter->GetCurrentUnit()->FinishTurn();

			return;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Enemy is out of range for ranged attack"));
		}
	}
	else if (!PlayerUnit->bIsRangedUnit)
	{
		if (ChebyshevDistance(PlayerPosition, EnemyPosition) > PlayerUnit->MovementSpeed + 1)
		{
			UE_LOG(LogTemp, Warning, TEXT("Enemy out of movement range |PlayerAttack_PlayerUnit.cpp|"));
			return;
		}

		MouseGridPos = GetMousePosition(PlayerCharacter->GetMouseWorldLocation(), PlayerCharacter->GetMouseWorldDirection());
		UE_LOG(LogTemp, Display, TEXT("Mouse pos X=%f, Y=%f, Enemy pos X=%f, Y=%f"), MouseGridPos.X, MouseGridPos.Y, TargetEnemyLocation.X, TargetEnemyLocation.Y);

		FVector2D AttackDirection = GetCardinalDirection(FVector2D(this->GetTargetLocation().X,this->GetTargetLocation().Y), MouseGridPos);
		FVector2D AttackTileGridPos = EnemyPosition + AttackDirection;
		UE_LOG(LogTemp, Display, TEXT("Attack tile pos X=%f, Y=%f"), AttackTileGridPos.X, AttackTileGridPos.Y);

		// Ensure GridManager is valid before accessing it
		if (!GridManager)
		{
			UE_LOG(LogTemp, Error, TEXT("GridManager is null before fetching attack tile"));
			return;
		}

		AGridTile* AttackTile = GridManager->GetTileAtPosition(AttackTileGridPos.X, AttackTileGridPos.Y);
		if (!AttackTile)
		{
			UE_LOG(LogTemp, Warning, TEXT("No valid attack tile found at X=%f, Y=%f |PlayerAttack_PlayerUnit.cpp|"), AttackTileGridPos.X, AttackTileGridPos.Y);
			return;
		}

		if (AttackTile->GridPosition == PlayerPosition)
		{
			UE_LOG(LogTemp, Warning, TEXT("Attack tile is the same as player position |PlayerAttack_PlayerUnit.cpp|"));
			PlayerUnit->EnemyToAttack = this;
			PlayerUnit->AttackAfterMovement();
			PlayerCharacter->GetCurrentUnit()->FinishTurn();
			return;
		}

		if (AttackTile->bIsOccupied)
		{
			UE_LOG(LogTemp, Warning, TEXT("Attack tile is occupied |PlayerAttack_PlayerUnit.cpp|"));
			return;
		}

		if (ChebyshevDistance(PlayerPosition, AttackTileGridPos) > PlayerUnit->MovementSpeed + 1)
		{
			UE_LOG(LogTemp, Warning, TEXT("Attack tile out of range |PlayerAttack_PlayerUnit.cpp|"));
			return;
		}

		if (AttackTile)
		{
			PlayerUnit->EnemyToAttack = this;
			PlayerUnit->OnMovementComplete.BindUObject(PlayerUnit, &APlayerUnit::AttackAfterMovement);
			PlayerUnit->MoveToTile(AttackTile->GridPosition);
		}
	}
}

FVector2D APlayerUnit::GetCardinalDirection(FVector2D EnemyPos, FVector2D MousePos)
{
	FVector2D Delta = MousePos - EnemyPos; // Get difference vector
	FVector2D Direction = FVector2D::ZeroVector;

	// Prioritize the dominant axis
	if (FMath::Abs(Delta.X) > FMath::Abs(Delta.Y))
	{
		Direction.X = (Delta.X > 0) ? -1 : 1;
	}
	else
	{
		Direction.Y = (Delta.Y > 0) ? 1 : -1;
	}

	return Direction;
}


void APlayerUnit::AttackAfterMovement()
{
	if (!combatManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("Combat Manager is null"));
		this->OnMovementComplete.Unbind();
		return;
	}
	if (EnemyToAttack)
	{
		combatManager->DealDamageToUnit(this, EnemyToAttack);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("nemyToAttack is null"));
	}
	this->OnMovementComplete.Unbind();
}

void APlayerUnit::ShootProjectile(FVector _EnemyLocation)
{
	if (ProjectileClass)
	{
		// Spawn the projectile
		FVector SpawnLocation = GetActorLocation(); // Start at the unit's location
		FVector EnemyLocation = _EnemyLocation; // Target location (e.g., enemy position)

		FRotator SpawnRotation = (EnemyLocation - SpawnLocation).Rotation();

		AGenericProjectile* Projectile = GetWorld()->SpawnActor<AGenericProjectile>(
			ProjectileClass, SpawnLocation, SpawnRotation);

		if (Projectile)
		{
			// Initialize the projectile with a curve
			Projectile->InitializeProjectileWithCurve(SpawnLocation, EnemyLocation);
		}
	}
}


void APlayerUnit::ExecutePlayerTurn()
{
	//some logic here
	UE_LOG(LogTemp, Error, TEXT("Player did something"));
	//CheckForItems();
	//FinishTurn();
}

void APlayerUnit::ExecuteAITurn()
{
	// Get the AI Controller for this pawn
	AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController());

	if (AIController)
	{
		// Call the MoveToGridPosition() function on the AI Controller
		AIController->MoveToGridPosition();

		// Log success
		UE_LOG(LogTemp, Log, TEXT("Called MoveToGridPosition from AI Controller."));
	}
	else
	{
		// Log failure if no AI Controller found
		UE_LOG(LogTemp, Warning, TEXT("Failed to find AI Controller."));
	}

	// Continue with the rest of the AI logic
	UE_LOG(LogTemp, Error, TEXT("AI did something"));

	// End the turn
	FinishTurn();
}

void APlayerUnit::FinishTurn()
{
	AGameManager* GameManager = Cast<AGameManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameManager::StaticClass()));

	if (!GameManager)
	{
		UE_LOG(LogTemp, Error, TEXT("FinishTurn() failed: GameManager is nullptr!"));
		return;
	}

	CheckForItems();
	GameManager->EndUnitTurn();
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

void APlayerUnit::ResetPosition()
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
	SetInitalPosition(GridStartPosition);
}
void APlayerUnit::EquipStartWeapon(AItem* ItemToAdd)
{
	if (!ItemToAdd)
	{
		UE_LOG(LogTemp, Error, TEXT("ItemToAdd is null"));
		return;
	}
	// Determine the appropriate slot index for the new item
	int32 SlotIndex = 0; // Default to weapon slot
	// Check if the slot is already occupied
	if (ItemSlots[SlotIndex])
	{
		AItem* OldItem = ItemSlots[SlotIndex];
		// Unequip the old item
		OldItem->DropItem();
		// Drop the old item back into the game world
		DropItem(OldItem, CurrentGridPosition);
	}
	// Equip the new item
	ItemSlots[SlotIndex] = ItemToAdd;
	ItemToAdd->EquipItem(this);
	//set the unit according to the item
	if (ItemToAdd->bIsMelee)
	{
		bIsRangedUnit = false;
	}
	else
	{
		bIsRangedUnit = true;
	}
	if (bIsRangedUnit)
	{
		UE_LOG(LogTemp, Error, TEXT("Player is a Ranged unit"));
	}

}

void APlayerUnit::CheckForItems()
{
	// Ensure the GridManager is valid
	if (!GridManager) {
		UE_LOG(LogTemp, Error, TEXT("NO MANAGER"));
		return;
	}

	GridManager->ScanWorldForObjects();
	// Retrieve the target tile based on the current grid position
	AActor* TargetTile = GridManager->GetTileAt(CurrentGridPosition.X, CurrentGridPosition.Y);

	if (!TargetTile) {
		UE_LOG(LogTemp, Warning, TEXT("No item at tile (%f, %f)"), CurrentGridPosition.X, CurrentGridPosition.Y);
		return;
	}

	// Cast the target tile to AGridTile
	AGridTile* Tile = Cast<AGridTile>(TargetTile);
	if (Tile && Tile->ItemSlot) {
		AItem* NewItem = Tile->ItemSlot;
		if (!NewItem) {
			UE_LOG(LogTemp, Error, TEXT("Item is null"));
			return;
		}

		// Determine the appropriate slot index for the new item
		int32 SlotIndex = 0; // Default to weapon slot
		

		// Check if the slot is already occupied
		if (ItemSlots[SlotIndex]) {
			AItem* OldItem = ItemSlots[SlotIndex];

			// Unequip the old item
			OldItem->DropItem();

			// Drop the old item back into the game world
			DropItem(OldItem, CurrentGridPosition);
		}

		// Equip the new item
		ItemSlots[SlotIndex] = NewItem;
		NewItem->EquipItem(this);
		Tile->ItemSlot = nullptr;
		//set the unit according to the item
		if (NewItem->bIsMelee) {
			bIsRangedUnit = false;

		}
		else {
			bIsRangedUnit = true;
		}
		
		if (bIsRangedUnit) {
			UE_LOG(LogTemp, Error, TEXT("Player is a Ranged unit"));

		}

		UE_LOG(LogTemp, Log, TEXT("Player picked up item at tile (%f, %f)"), CurrentGridPosition.X, CurrentGridPosition.Y);
	}


}

void APlayerUnit::CalculateStats()
{

}

void APlayerUnit::DropItem(AItem* OldItem, FVector2D CurrentPosition)
{
	if (!OldItem)
	{
		UE_LOG(LogTemp, Error, TEXT("OldItem is null"));
		return;
	}

	// Remove the item from the player's inventory
	ItemSlots[0] = nullptr;
	if (OldItem->IsValidLowLevel())
	{
		OldItem->Destroy();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("OldItem is not valid"));
	}
	// Determine the world location to drop the item
	FVector DropLocation = GetActorLocation(); // Adjust the offset as needed

	// Spawn parameters
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	// Spawn the item actor in the world
	AItem* DroppedItem = GetWorld()->SpawnActor<AItem>(OldItem->GetClass(), DropLocation, FRotator::ZeroRotator, SpawnParams);
	if (!DroppedItem)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn dropped item"));
		return;
	}

	// If using a grid system, update the corresponding tile
	if (GridManager)
	{
		AActor* TargetTile = GridManager->GetTileAt(CurrentPosition.X, CurrentPosition.Y);
		AGridTile* Tile = Cast<AGridTile>(TargetTile);
		if (Tile)
		{
			Tile->ItemSlot = DroppedItem;
			UE_LOG(LogTemp, Log, TEXT("Updated tile with dropped item"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to update tile with dropped item"));
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Player dropped item at location (%s)"), *DropLocation.ToString());

}

void APlayerUnit::BeginFocus()
{
	this->UpdateInteractableData();
	if (SkeletalMesh)
	{
		SkeletalMesh->SetRenderCustomDepth(true);
	}
	// Store movement tiles in MovementTiles
	MovedTiles = GridManager->GetMovableTiles(CurrentGridPosition.X, CurrentGridPosition.Y, MovementSpeed);
	for (auto tile : MovedTiles)
	{
		if (bIsPlayerUnit)
		{
			tile->GreenHighlight();
		}
		else
		{
			tile->RedHighlight();
		}
		tile->BeginFocus();
	}
}

void APlayerUnit::EndFocus()
{
	if (SkeletalMesh)
	{
		SkeletalMesh->SetRenderCustomDepth(false);
	}
	// Process MovementTiles instead of MovedTiles
	for (auto tile : MovedTiles)
	{
		tile->EndFocus();
	}
	MovedTiles.Empty();
}

void APlayerUnit::BeginMouseHoverFocus()
{
	if(!this->bIsCurrentUnit)
	{
		SkeletalMesh->SetRenderCustomDepth(true);
		if (GridManager && !bIsCurrentUnit)
		{
			// Use HoverTiles instead of MovedTiles
			HoverTiles = GridManager->GetMovableTiles(CurrentGridPosition.X, CurrentGridPosition.Y, MovementSpeed);
			for (AGridTile* Tile : HoverTiles)
			{
				if (bIsPlayerUnit)
				{
					if (Tile->bIsGreenHighlighted || Tile->bIsRedHighlighted)
					{
						Tile->YellowHighlight();
					}
					else
					{
						Tile->GreenHighlight();
					}
				}
				else
				{
					if (Tile->bIsGreenHighlighted || Tile->bIsRedHighlighted)
					{
						Tile->YellowHighlight();
					}
					else
					{
						Tile->RedHighlight();
					}
				}
			}
		}
	}
}

void APlayerUnit::EndMouseHoverFocus()
{
	if(!this->bIsCurrentUnit)
	{
		SkeletalMesh->SetRenderCustomDepth(false);
		for (AGridTile* Tile : HoverTiles)
		{
			if (Tile)
			{
				Tile->EndMouseHoverFocus();
			}
		}
		HoverTiles.Empty();
	}
}


void APlayerUnit::Interact(APlayerCamera* PlayerCharacter)
{
	if (PlayerCharacter)
	{
	
		this->UpdateInteractableData();

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
	else if (bIsPlayerUnit && bIsRangedUnit) {
		InstanceInteractableData.UnitName = FText::FromString("BowMan");
	}
	else
	{
		InstanceInteractableData.UnitName = FText::FromString("Rat");
	}

	InstanceInteractableData.UnitHealth = Health;
	InstanceInteractableData.UnitDamage = Damage;
	InstanceInteractableData.UnitMovementSpeed = MovementSpeed;

}