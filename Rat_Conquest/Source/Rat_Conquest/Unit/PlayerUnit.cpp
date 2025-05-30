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
#include "Rat_Conquest/Items/Weapon.h"
#include "Rat_Conquest/AI/EnemyAIController.h"
#include "Rat_Conquest/Projectiles/GenericProjectile.h"
#include "Rat_Conquest/Data/MutationData.h"
#include "Rat_Conquest/Widgets/MainHUD.h"
#include "Components/WidgetComponent.h"
#include "Rat_Conquest/Items/ItemBase.h"
#include "Rat_Conquest/Widgets/HealthBar.h"
#include "Rat_Conquest/LevelObjects/InteractableGridObject.h"
#include "Sound/SoundBase.h"


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


	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(SkeletalMesh, TEXT("Hand_R")); 
	WeaponMesh->SetRelativeScale3D(FVector(1.0f));

	ArmorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArmorMesh"));
	ArmorMesh->SetupAttachment(SkeletalMesh, TEXT("Chest"));

	OutfitMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OutfitMesh"));
	OutfitMesh->SetupAttachment(SkeletalMesh, TEXT("Chest"));

	HelmetMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HelmetMesh"));
	HelmetMesh->SetupAttachment(SkeletalMesh, TEXT("Head"));


	ArmorMesh->SetRelativeScale3D(FVector(1.0f));
	HealthBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidgetComponent"));
	HealthBarWidgetComponent->SetupAttachment(RootComponent);
	HealthBarWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	
	

	UE_LOG(LogTemp, Warning, TEXT("SkeletalMesh Created and Collision Set"));

	// Initialize other properties
	ItemSlots.SetNum(3);
	GridStartPosition = FVector2D(0, 0);
	GridManager = nullptr;
	CombatManager = nullptr;
	bIsRangedUnit = false;
	Health = maxHealth;

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
		CombatManager = Cast<ACombatManager>(FoundCombatManagers[0]);
		if (FoundCombatManagers.Num() > 1)
		{
			UE_LOG(LogTemp, Warning, TEXT("Multiple Combatmanagers found! Using first instance."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No CombatManager found in the level!"));
	}

		GameManager = Cast<AGameManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameManager::StaticClass()));

		if (!GameManager.IsValid())
		{
			UE_LOG(LogTemp, Error, TEXT("BeginPlay(): No GameManager found!"));
		}

	DelayedInitalPosition();

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

	animationToPlay = FVector2D(0, 0);

	mutationData = std::make_shared<MutationData>();
	mutationData->SetExperienceNeeded(10);

	UnitName = FName("PlayerUnit");

	if (ItemSlots.Num() == 0)
	{
		ItemSlots.Add(nullptr);
		ItemSlots.Add(nullptr);
		ItemSlots.Add(nullptr);
	}

	if (!HealthBarWidgetComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("HealthBarWidgetComponent NOT found! Make sure it exists in the Blueprint."));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("HealthBarWidgetComponent found!"));

	if (HealthBarWidgetComponent->GetWidgetClass())
	{
		HealthBarWidget = Cast<UHealthBar>(HealthBarWidgetComponent->GetUserWidgetObject());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("HealthBarWidgetComponent has no WidgetClass! Make sure it's set in the Blueprint."));
	}

	RandomizeStats();
	UpdateHealthBar(); // Update UI initially
	this->UpdateInteractableData();
}

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

			// Snap to final position
			SetActorLocation(FVector(TargetPosition.X, TargetPosition.Y, GetActorLocation().Z));
			UE_LOG(LogTemp, Display, TEXT("Finished moving to tile: %s"), *TargetPosition.ToString());

			// Update grid position
			CurrentGridPosition = TargetGridPosition;

			// Continue movement if there are more tiles
			if (PathToTake.Num() > 0)
			{
				// Get the next tile
				FVector2D NextTilePosition = PathToTake[0];
				PathToTake.RemoveAt(0);

				AActor* NextTile = GridManager->GetTileAt(NextTilePosition.X, NextTilePosition.Y).Get();
				if (NextTile)
				{
					StartPosition = GetActorLocation();
					TargetPosition = NextTile->GetActorLocation();
					TargetGridPosition = NextTilePosition;
					MovementProgress = 0.0f;
					bIsMoving = true;
				}
			}
			else
			{
				// Movement finished
				UE_LOG(LogTemp, Display, TEXT("Finished moving along the path."));

				if (EnemyToAttack.IsValid())
				{
					// Face the enemy
					FVector EnemyDirection = (EnemyToAttack->GetActorLocation() - GetActorLocation()).GetSafeNormal();
					FRotator EnemyRotation = EnemyDirection.Rotation();
					SetActorRotation(EnemyRotation);
				}
				else
				{
					// Face forward (default)
					if (!bIsPlayerUnit)
						SetActorRotation(FRotator(0, TurnAngle + 180, 0));
					else
						SetActorRotation(FRotator(0, TurnAngle, 0));
				}

				// Trigger animation reset
				animationToPlay = FVector2D(0, 0);

				// Notify movement completion
				if (OnMovementComplete.IsBound())
				{
					OnMovementComplete.Execute();
					OnMovementComplete.Unbind();
				}

				// Finish turn if player-controlled

				this->FinishTurn();
			}
		}
		else
		{
			// Interpolate movement
			FVector NewPosition = FMath::Lerp(StartPosition, TargetPosition, MovementProgress);
			SetActorLocation(FVector(NewPosition.X, NewPosition.Y, GetActorLocation().Z));

			// Ensure the unit rotates correctly (only up/down/left/right)
			FVector Direction = TargetPosition - StartPosition;
			FRotator TargetRotation = GetActorRotation();

			if (FMath::Abs(Direction.X) > FMath::Abs(Direction.Y))
			{
				// Moving Left or Right
				TargetRotation.Yaw = (Direction.X > 0) ? 0.0f : 180.0f; // Right (0�), Left (180�)
			}
			else
			{
				// Moving Up or Down
				TargetRotation.Yaw = (Direction.Y > 0) ? 90.0f : -90.0f; // Up (90�), Down (-90�)
			}

			SetActorRotation(TargetRotation);
		}
	}

	// Handle knockback movement
	if (bIsKnockbackActive)
	{
		KnockbackProgress += DeltaTime / KnockbackDuration;

		if (KnockbackProgress >= 1.0f)
		{
			SetActorLocation(KnockbackTargetPosition);
			bIsKnockbackActive = false;
		}
		else
		{
			FVector NewPosition = FMath::Lerp(KnockbackStartPosition, KnockbackTargetPosition, KnockbackProgress);
			SetActorLocation(NewPosition);
		}
	}

	UpdateHealthBarRotation();
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

void APlayerUnit::ResetRotation()
{
	if (!bIsPlayerUnit)
		SetActorRotation(FRotator(0, TurnAngle + 180, 0));
	else
		SetActorRotation(FRotator(0, TurnAngle, 0));
}


void APlayerUnit::NotifyBlendSpaceChange()
{
	UE_LOG(LogTemp, Display, TEXT("NotifyBlendSpaceChange called"));
		this->OnBlendSpaceChange.Broadcast();
}

void APlayerUnit::MoveToTile(FVector2D NewGridPosition)
{
	if (!bIsCurrentUnit && bIsPlayerUnit)
		return;
	if (!GridManager.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("NO MANAGER |MoveToTile_PlayerUnit.cpp|"));
		return;
	}

	AActor* TargetTile = GridManager->GetTileAt(NewGridPosition.X, NewGridPosition.Y).Get();
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
		if (!bIsPlayerUnit)
		{
			this->FinishTurn();
		}
		return;
	}

	// Free the current tile
	AActor* OldTile = GridManager->GetTileAt(CurrentGridPosition.X, CurrentGridPosition.Y).Get();
	AGridTile* OldGridTile = Cast<AGridTile>(OldTile);
	if (!OldGridTile)
	{
		UE_LOG(LogTemp, Error, TEXT("OldGridTile is null |MoveToTile_PlayerUnit.cpp|"));
		return;
	}

	OldGridTile->bIsOccupied = false;
	OldGridTile->RemoveUnitReference();

	// Check if the target tile is within movement range
	float distance = ChebyshevDistance(GridTile->GridPosition, OldGridTile->GridPosition);
	if (distance > FMath::Max(1,MovementSpeed + SpeedFromItems))
	{
		UE_LOG(LogTemp, Warning, TEXT("Target tile is out of movement range |MoveToTile_PlayerUnit.cpp|"));
		if (!bIsPlayerUnit)
		{
			this->FinishTurn();
		}
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
	GridTile->SetUnitReference(this);
	if (SB_Walk) {
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), SB_Walk, GetActorLocation(), GetActorRotation());
	}
	
	// Set up movement to the first tile in the path
	if (PathToTake.Num() > 0)
	{
		FVector2D NextTilePosition = PathToTake[0];
		PathToTake.RemoveAt(0);

		AActor* NextTile = GridManager->GetTileAt(NextTilePosition.X, NextTilePosition.Y).Get();
		if (NextTile)
		{
			StartPosition = GetActorLocation();
			TargetPosition = NextTile->GetActorLocation();
			TargetGridPosition = NextTilePosition;
			MovementProgress = 0.0f; // Reset progress
			bIsMoving = true;        // Start movement

			//PlayWalkAnimation
			animationToPlay = FVector2D(25, 0);
		}
	}
}

bool APlayerUnit::CalculatePathToTile(FVector2D InTargetGridPosition)
{
	if (!GridManager.IsValid())
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
		TArray<AGridTile*> NeighbourTiles;
		NeighbourTiles.Add(GridManager->GetTileAtPosition(CurrentTile->GridPosition.X + 1, CurrentTile->GridPosition.Y).Get()); // Right
		NeighbourTiles.Add(GridManager->GetTileAtPosition(CurrentTile->GridPosition.X - 1, CurrentTile->GridPosition.Y).Get()); // Left
		NeighbourTiles.Add(GridManager->GetTileAtPosition(CurrentTile->GridPosition.X, CurrentTile->GridPosition.Y + 1).Get()); // Up
		NeighbourTiles.Add(GridManager->GetTileAtPosition(CurrentTile->GridPosition.X, CurrentTile->GridPosition.Y - 1).Get()); // Down


		for (AGridTile* Neighbour : NeighbourTiles)
		{
			if (!Neighbour || Neighbour->bIsOccupied || ClosedList.Contains(Neighbour))
			{
				continue; // Skip occupied or already evaluated tiles
			}

			float TentativeG = CurrentTile->G + ChebyshevDistance(CurrentTile->GridPosition, Neighbour->GridPosition);

			if (TentativeG > FMath::Max(1, MovementSpeed + SpeedFromItems))
			{
				continue;
			}

			if (!OpenList.Contains(Neighbour) || TentativeG < Neighbour->G)
			{
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
	if (!GridManager.IsValid()) {
		UE_LOG(LogTemp, Error, TEXT("NO MANAGER |SetInitalPosition_PlayerUnit.cpp|"));
		return;
	}
	AActor* TargetTile;
	TargetTile = GridManager->SetStartingPositions(bIsPlayerUnit).Get();
	if (!TargetTile) {
		UE_LOG(LogTemp, Warning, TEXT("Invalid target tile at (%f, %f) |SetInitalPosition_PlayerUnit.cpp|"), position.X, position.Y);
		TargetTile = GridManager->GetClosestAvailableTile(position).Get();
		if (TargetTile) {
			UE_LOG(LogTemp, Warning, TEXT("Target tile was set to default"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Target tile was not set"));
			return;
		}
	}
	
	AGridTile* TargetGridTile = Cast<AGridTile>(TargetTile);
	if (!TargetGridTile) {
		return;
	}

	TargetPosition = TargetTile->GetActorLocation();
	SetActorLocation(FVector(TargetPosition.X, TargetPosition.Y, GetActorLocation().Z));
	CurrentGridPosition = TargetGridTile->GridPosition;
	//set the unit pointer on the grid tile
	AGridTile* GridTile = Cast<AGridTile>(TargetTile);
	if (GridTile) {
		GridTile->SetUnitReference(this);
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
	
	if (GridManager.IsValid() && GridManager->IsGridFinished() && GridManager->IsGridScanned())
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

void APlayerUnit::PlayerAttack(TWeakObjectPtr<APlayerCamera> PlayerCharacter)
{
	// Define constants
	constexpr int ADJACENT_TILE_RANGE = 1;

	// Validate inputs
	if (!PlayerCharacter.IsValid() || !GridManager.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid PlayerCharacter or GridManager in PlayerAttack"));
		return;
	}

	const TWeakObjectPtr<APlayerUnit> PlayerUnit = PlayerCharacter->GetCurrentUnit();
	if (!PlayerUnit.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid PlayerUnit in PlayerAttack"));
		return;
	}

	const FVector2D PlayerPosition = PlayerUnit->CurrentGridPosition;
	const FVector2D EnemyPosition = CurrentGridPosition;
	TargetEnemyLocation = EnemyPosition;

	const float DistanceToEnemy = ChebyshevDistance(EnemyPosition, PlayerPosition);

	// Handle ranged attack
	if (PlayerUnit->bIsRangedUnit)
	{
		if (DistanceToEnemy > AttackRange + RangeFromItems)
		{
			UE_LOG(LogTemp, Warning, TEXT("Enemy is out of range for ranged attack"));
			return;
		}

		PlayerUnit->EnemyToAttack = this;
		PlayerUnit->UseCurrentItem();
		return;
	}

	// Handle melee attack
	if (DistanceToEnemy > FMath::Max(1,PlayerUnit->MovementSpeed + PlayerUnit->SpeedFromItems + ADJACENT_TILE_RANGE))
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy out of movement range"));
		return;
	}

	MouseGridPos = GetMousePosition(PlayerCharacter->GetMouseWorldLocation(), PlayerCharacter->GetMouseWorldDirection());
	UE_LOG(LogTemp, Display, TEXT("Mouse pos X=%f, Y=%f, Enemy pos X=%f, Y=%f"),
		MouseGridPos.X, MouseGridPos.Y, TargetEnemyLocation.X, TargetEnemyLocation.Y);

	const FVector2D AttackDirection = GetCardinalDirection(
		FVector2D(GetTargetLocation().X, GetTargetLocation().Y),
		MouseGridPos);
	UE_LOG(LogTemp, Display, TEXT("Cardinal Direction when attacking: %s"), *AttackDirection.ToString());

	const FVector2D AttackTileGridPos = EnemyPosition + AttackDirection;
	UE_LOG(LogTemp, Display, TEXT("Attack tile pos X=%f, Y=%f"), AttackTileGridPos.X, AttackTileGridPos.Y);

	AGridTile* AttackTile = GridManager->GetTileAtPosition(AttackTileGridPos.X, AttackTileGridPos.Y).Get();
	if (!AttackTile)
	{
		UE_LOG(LogTemp, Warning, TEXT("No valid attack tile found"));
		return;
	}

	if (AttackTile->GridPosition == PlayerPosition)
	{
		PlayerUnit->EnemyToAttack = this;
		PlayerUnit->AttackAfterMovement();
		return;
	}

	if (AttackTile->bIsOccupied)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attack tile is occupied"));
		return;
	}

	if (ChebyshevDistance(PlayerPosition, AttackTileGridPos) > FMath::Max(1,PlayerUnit->MovementSpeed + SpeedFromItems + ADJACENT_TILE_RANGE))
	{
		UE_LOG(LogTemp, Warning, TEXT("Attack tile out of range"));
		return;
	}

	PlayerUnit->EnemyToAttack = this;
	PlayerUnit->OnMovementComplete.BindUObject(PlayerUnit.Get(), &APlayerUnit::AttackAfterMovement);
	PlayerUnit->MoveToTile(AttackTile->GridPosition);
}


FVector2D APlayerUnit::GetCardinalDirection(FVector2D EnemyPos, FVector2D MousePos)
{
	FVector2D Delta = MousePos - EnemyPos; // Get difference vector
	FVector2D Direction = FVector2D::ZeroVector;

	if (FMath::Abs(Delta.X) > FMath::Abs(Delta.Y))
	{
		Direction.X = (Delta.X > 0) ? -1 : 1;
	}
	else if (FMath::Abs(Delta.Y) > 0)  // Ensure non-zero
	{
		Direction.Y = (Delta.Y > 0) ? 1 : -1;
	}

	// If both X and Y are zero, set a default direction (e.g., Up)
	if (Direction.IsZero())
	{
		UE_LOG(LogTemp, Warning, TEXT("GetCardinalDirection: Delta is zero! Defaulting to Up (0, -1)"));
		Direction = FVector2D(0, -1);  // Default to 'Up' to prevent errors
	}

	return Direction;
}



void APlayerUnit::AttackAfterMovement()
{
	if (!CombatManager.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Combat Manager is null"));
		this->OnMovementComplete.Unbind();
		return;
	}

	if (EnemyToAttack.IsValid())
	{
		//check distance to enemy
		const float DistanceToEnemy = ChebyshevDistance(CurrentGridPosition, EnemyToAttack->CurrentGridPosition);
		if (DistanceToEnemy > 1)
		{
			UE_LOG(LogTemp, Warning, TEXT("Enemy is out of range for attack"));
			this->OnMovementComplete.Unbind();
			return;
		}
		if (SB_Attack)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), SB_Attack, GetActorLocation(), GetActorRotation());
		}

		animationToPlay = FVector2D(50, 0);
		CombatManager->DealDamageToUnit(this, EnemyToAttack.Get());
		this->FinishTurn();

		// Delay ResetRotation by 1 second
		FTimerHandle RotationResetHandle;
		GetWorld()->GetTimerManager().SetTimer(RotationResetHandle, this, &APlayerUnit::ResetRotation, 1.0f, false );

		EnemyToAttack = nullptr;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("EnemyToAttack is null"));
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
	bHasAttacked = false;
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
		AIController->ChooseAction();

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
	//this->FinishTurn();
}

void APlayerUnit::FinishTurn()
{
	if (!GameManager.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("GameManager is null"));
		return;
	}

	if (bHasFinishedTurn)
	{
		UE_LOG(LogTemp, Warning, TEXT("FinishTurn() already called this turn!"));
		return;
	}

	CheckForItems();
	CheckForGridObjects();
	GameManager->EndUnitTurn();

	bHasFinishedTurn = true; // Prevent duplicate turn endings
}


void APlayerUnit::DestoryUnit()
{
	if (GridManager.IsValid())
	{
		AActor* Tile = GridManager->GetTileAt(CurrentGridPosition.X, CurrentGridPosition.Y).Get();
		AGridTile* GridTile = Cast<AGridTile>(Tile);
		if (GridTile)
		{
			GridTile->RemoveUnitReference();
			GridTile->bIsOccupied = false;
			UE_LOG(LogTemp, Error, TEXT("AHH YOU KILLED ME!! from grid tile at (%f, %f)"), CurrentGridPosition.X, CurrentGridPosition.Y);
			
		}
	}
	Destroy(true, true);
}

void APlayerUnit::ResetPosition()
{
	if (GridManager.IsValid())
	{
		AActor* Tile = GridManager->GetTileAt(CurrentGridPosition.X, CurrentGridPosition.Y).Get();
		AGridTile* GridTile = Cast<AGridTile>(Tile);
		if (GridTile)
		{
			GridTile->RemoveUnitReference();
			GridTile->bIsOccupied = false;
		}
	}
	SetInitalPosition(GridStartPosition);
}
void APlayerUnit::EquipStartWeapon(TWeakObjectPtr<AItem> ItemToAdd)
{
	// Validate input
	if (!ItemToAdd.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Attempted to equip null item"));
		return;
	}

	// Ensure we have item slots
	constexpr int32 WEAPON_SLOT_INDEX = 0;
	if (ItemSlots.Num() <= WEAPON_SLOT_INDEX)
	{
		UE_LOG(LogTemp, Error, TEXT("ItemSlots array not properly initialized"));
		return;
	}

	// Handle existing item
	if (ItemSlots[WEAPON_SLOT_INDEX].IsValid())
	{
		if (AItem* OldItem = ItemSlots[WEAPON_SLOT_INDEX].Get())
		{
			OldItem->DropItem();
			DropItem(OldItem, CurrentGridPosition);
		}
	}

	// Equip new item
	ItemSlots[WEAPON_SLOT_INDEX] = ItemToAdd;
	ItemToAdd->EquipItem(this);

	// Update unit type based on weapon
	bIsRangedUnit = !ItemToAdd->bIsMelee;

#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Log, TEXT("Equipped %s weapon: %s"),
		bIsRangedUnit ? TEXT("Ranged") : TEXT("Melee"),
		*ItemToAdd->GetName());
#endif
}

void APlayerUnit::UseCurrentItem()
{

	if (ItemSlots[0].IsValid() && !bIsMoving && !bHasAttacked) { // use weapon to attack
		AWeapon* currentWeapon = Cast<AWeapon>(ItemSlots[0]);
		if (currentWeapon && IsValid(EnemyToAttack.Get())) {
			currentWeapon->SetEnemyLocation(EnemyToAttack->GetActorLocation());
			currentWeapon->SetUnitRefrence(this, EnemyToAttack);
			currentWeapon->UseItem();
			if (bIsRangedUnit) {
				float CurrentDelay = currentWeapon->ProjectileTime;
				
				if (IsValid(CombatManager.Get()) && IsValid(EnemyToAttack.Get())) {
					//CombatManager->DealDamageToUnit(this, EnemyToAttack.Get());
					//FinishTurn();
				}
			}
			else {
				CombatManager->DealDamageToUnit(this, EnemyToAttack.Get());
				
			}

			EnemyToAttack = nullptr;
			bHasAttacked = true;
			UE_LOG(LogTemp, Error, TEXT("RANGED ATTACK"))
		}
		

	}
}

void APlayerUnit::TakeDamageFromEnemy(int DamageTaken)
{
	if (!CombatManager.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("CombatManager is null"));
		return;
	}
	CombatManager->HandleUnitDamage(this, DamageTaken);
}

void APlayerUnit::CheckForGridObjects()
{
	if (!IsValid(GridManager.Get())) {
		UE_LOG(LogTemp, Error, TEXT("NO MANAGER"));
		return;
	}
	GridManager->ScanWorldForObjects();
	// Retrieve the target tile based on the current grid position
	AActor* TargetTile = GridManager->GetTileAt(CurrentGridPosition.X, CurrentGridPosition.Y).Get();

	if (!TargetTile) {
		UE_LOG(LogTemp, Warning, TEXT("No item at tile (%f, %f)"), CurrentGridPosition.X, CurrentGridPosition.Y);
		return;
	}
	TWeakObjectPtr<AGridTile> Tile = Cast<AGridTile>(TargetTile);
	TWeakObjectPtr<AInteractableGridObject> InteractableObject = Cast<AInteractableGridObject>(Tile->InteractableObjectSlot);
	if (Tile.IsValid() && Tile->InteractableObjectSlot.IsValid()) {
		UE_LOG(LogTemp, Error, TEXT("Found Grid object"));
		InteractableObject->InteractWithObject(this);
	}
}

void APlayerUnit::CheckForItems()
{
	// Ensure the GridManager is valid
	if (!IsValid(GridManager.Get())) {
		UE_LOG(LogTemp, Error, TEXT("NO MANAGER"));
		return;
	}

	GridManager->ScanWorldForObjects();
	// Retrieve the target tile based on the current grid position
	AActor* TargetTile = GridManager->GetTileAt(CurrentGridPosition.X, CurrentGridPosition.Y).Get();

	if (!TargetTile) {
		UE_LOG(LogTemp, Warning, TEXT("No item at tile (%f, %f)"), CurrentGridPosition.X, CurrentGridPosition.Y);
		return;
	}

	// Cast the target tile to AGridTile
	AGridTile* Tile = Cast<AGridTile>(TargetTile);
	if (Tile && Tile->ItemSlot.IsValid()) {
		AItem* NewItem = Tile->ItemSlot.Get();
		if (!NewItem) {
			UE_LOG(LogTemp, Error, TEXT("Item is null"));
			return;
		}
		int32 SlotIndex = 0;
		if (NewItem->ItemDataB->ItemType == EItemType::Weapon)
		{
			UE_LOG(LogTemp, Error, TEXT("Player picked up weapon"));
		}
		else if (NewItem->ItemDataB->ItemType == EItemType::Armor)
		{
			SlotIndex = 1; // Armor slot
			if (!bIsPlayerUnit)
				return;
			UE_LOG(LogTemp, Error, TEXT("Player picked up armor"));
		}
		else if (NewItem->ItemDataB->ItemType == EItemType::Helmet)
		{
			SlotIndex = 2; // Consumable slot
			if (!bIsPlayerUnit)
				return;
			UE_LOG(LogTemp, Error, TEXT("Player picked up consumable"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Unknown item type"));
			return;
		}
		
		// Determine the appropriate slot index for the new item
		 // Default to weapon slot
		

		// Check if the slot is already occupied
		if (ItemSlots[SlotIndex].IsValid()) {
			AItem* OldItem = ItemSlots[SlotIndex].Get();

			// Unequip the old item
			OldItem->DropItem();

			// Drop the old item back into the game world
			DropItem(OldItem, CurrentGridPosition);
		}

		// Equip the new item
		ItemSlots[SlotIndex] = NewItem;
		if (SB_PickupItem) {
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), SB_PickupItem, GetActorLocation(), GetActorRotation());
		}
		NewItem->EquipItem(this);
		if (NewItem->ItemDataB->ItemType == EItemType::Weapon)
		{
			// Set the weapon mesh
			WeaponMesh->SetStaticMesh(NewItem->ItemMesh->GetStaticMesh());
		}
		else if (NewItem->ItemDataB->ItemType == EItemType::Armor)
		{
			// Set the armor mesh
			OutfitMesh->SetStaticMesh(NewItem->ItemMesh->GetStaticMesh());
		}
		else if (NewItem->ItemDataB->ItemType == EItemType::Helmet)
		{
			// Set the artifact mesh
			//ItemMesh->SetStaticMesh(NewItem->ItemMesh->GetStaticMesh());
			HelmetMesh->SetStaticMesh(NewItem->ItemMesh->GetStaticMesh());
		}
		


		if(NewItem->ItemDataB->ItemAssetData.BlendSpace)
		{
			CurrentAnimInstance = NewItem->ItemDataB->ItemAssetData.BlendSpace;
			this->NotifyBlendSpaceChange();
		}

		//if (NewItem->ItemDataB->ItemType == EItemType::Weapon)
		//{
		//	// Equip the new item
		//	Weapon = NewItem;
		//}
		Tile->ItemSlot = nullptr;
		//set the unit according to the item
		if (NewItem->bIsMelee && NewItem->ItemDataB->ItemType == EItemType::Weapon) {
			bIsRangedUnit = false;

		}
		else if (!NewItem->bIsMelee && NewItem->ItemDataB->ItemType == EItemType::Weapon) {
			bIsRangedUnit = true;
		}
		
		if (bIsRangedUnit) {
			UE_LOG(LogTemp, Error, TEXT("Player is a Ranged unit"));

		}

		UE_LOG(LogTemp, Log, TEXT("Player picked up item at tile (%f, %f)"), CurrentGridPosition.X, CurrentGridPosition.Y);
	}
	CalculateStats();
	UpdateInteractableData();

}

void APlayerUnit::CalculateStats()
{
	// Calculate stats based on equipped items
	DamageFromItems = 0;
	DefenceFromItems = 0;
	AttackFromItems = 0;
	RangeFromItems = 0;
	InitiativeFromItems = 0;
	SpeedFromItems = 0;
	MaxHealthFromItems = 0;



	if (ItemSlots[0].IsValid())
	{
		AItem* CurrentItem = ItemSlots[0].Get();
		if (CurrentItem)
		{
			DamageFromItems += CurrentItem->Damage;
			DefenceFromItems += CurrentItem->Defence;
			AttackFromItems += CurrentItem->Attack;
			RangeFromItems += CurrentItem->Range;
			InitiativeFromItems += CurrentItem->Initiative;
			SpeedFromItems += CurrentItem->Movement;
			MaxHealthFromItems += CurrentItem->Health;
			




		}
	}
	if (ItemSlots[1].IsValid()) {
		AItem* CurrentItem = ItemSlots[1].Get();
		if (CurrentItem)
		{
			DamageFromItems += CurrentItem->Damage;
			DefenceFromItems += CurrentItem->Defence;
			AttackFromItems += CurrentItem->Attack;
			RangeFromItems += CurrentItem->Range;
			InitiativeFromItems += CurrentItem->Initiative;
			SpeedFromItems += CurrentItem->Movement;
			MaxHealthFromItems += CurrentItem->Health;
		}

	}
	if (ItemSlots[2].IsValid()) {
		AItem* CurrentItem = ItemSlots[2].Get();
		if (CurrentItem)
		{
			DamageFromItems += CurrentItem->Damage;
			DefenceFromItems += CurrentItem->Defence;
			AttackFromItems += CurrentItem->Attack;
			RangeFromItems += CurrentItem->Range;
			InitiativeFromItems += CurrentItem->Initiative;
			SpeedFromItems += CurrentItem->Movement;
			MaxHealthFromItems += CurrentItem->Health;
		}

	}
	

}

void APlayerUnit::DropItem(TWeakObjectPtr<AItem> OldItem, FVector2D CurrentPosition)
{
	if (!IsValid(OldItem.Get()))
	{
		UE_LOG(LogTemp, Error, TEXT("OldItem is null"));
		return;
	}

	// Remove the item from the player's inventory
	
	// Determine the world location to drop the item
	FVector DropLocation = GetActorLocation(); // Adjust the offset as needed
	DropLocation.Z += 45;
	// Spawn parameters
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	// Spawn the item actor in the world
	if (!OldItem.IsValid())
		return;
	AItem* DroppedItem = GetWorld()->SpawnActor<AItem>(OldItem->GetClass(), DropLocation, FRotator::ZeroRotator, SpawnParams);
	
	if (!DroppedItem)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn dropped item"));
		return;
	}
	if (DroppedItem->ItemDataB->ItemType == EItemType::Weapon) {

		ItemSlots[0] = nullptr;
	}
	else if (DroppedItem->ItemDataB->ItemType == EItemType::Armor) {
		ItemSlots[1] = nullptr;
	}
	else if (DroppedItem->ItemDataB->ItemType == EItemType::Helmet) {
		ItemSlots[2] = nullptr;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Unknown item type"));
		return;

	}
	if (OldItem->IsValidLowLevel())
	{
		OldItem->Destroy();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("OldItem is not valid"));
	}
	// If using a grid system, update the corresponding tile
	if (IsValid(GridManager.Get()))
	{
		AActor* TargetTile = GridManager->GetTileAt(CurrentPosition.X, CurrentPosition.Y).Get();
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

	if (AnimBlueprint)
	{
		CurrentAnimInstance = AnimBlueprint; 
		this->NotifyBlendSpaceChange();
	}
	UE_LOG(LogTemp, Log, TEXT("Player dropped item at location (%s)"), *DropLocation.ToString());

}

void APlayerUnit::KillAfterAnim()
{
	//ToBE changed. Run DestroyUnit() after 3 secound
	animationToPlay = FVector2D(100, 0);
	GetWorldTimerManager().SetTimer(DeathTimer, this, &APlayerUnit::DestoryUnit, 1.0f, false);
}

void APlayerUnit::BeginFocus()
{
	this->UpdateInteractableData();
	if (SkeletalMesh)
	{
	//	SkeletalMesh->SetRenderCustomDepth(true);
	}
	// Store movement tiles in MovementTiles
	MovedTiles = GridManager->GetMovableTiles(CurrentGridPosition.X, CurrentGridPosition.Y, FMath::Max(1,MovementSpeed + SpeedFromItems));
	for (auto tile : MovedTiles)
	{
		//if (bIsPlayerUnit)
		//{
			tile->GreenHighlight();
		//}
		//else
		//{
		//	tile->RedHighlight();
		//}
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
		if (IsValid(tile.Get()))
		tile->EndFocus();
	}
	MovedTiles.Empty();
}

void APlayerUnit::BeginMouseHoverFocus()
{
	// Early return if this is the current unit or skeletal mesh is invalid
	if (bIsCurrentUnit || !SkeletalMesh)
	{
		return;
	}

	// Enable custom depth rendering
	SkeletalMesh->SetRenderCustomDepth(true);

	// Get movable tiles if grid manager is valid
	if (GridManager.IsValid())
	{
		// Clear previous hover tiles first
		for (auto& Tile : HoverTiles)
		{
			if (Tile.IsValid())
			{
				Tile->EndHighlight();
			}
		}

		// Get new movable tiles
		HoverTiles = GridManager->GetMovableTiles(CurrentGridPosition.X, CurrentGridPosition.Y, FMath::Max(1, MovementSpeed + SpeedFromItems));

		// Highlight tiles based on unit type
		for (auto& Tile : HoverTiles)
		{
			if (!Tile.IsValid()) continue;

			// Choose highlight color based on unit type
			//const bool bShouldRedHighlight = !bIsPlayerUnit && !(Tile->bIsGreenHighlighted || Tile->bIsRedHighlighted);
			//const bool bShouldGreenHighlight = bIsPlayerUnit && !(Tile->bIsGreenHighlighted || Tile->bIsRedHighlighted);

			if (Tile->bIsGreenHighlighted || Tile->bIsRedHighlighted)
			{
				Tile->YellowHighlight();
			}
			//else if (bShouldRedHighlight)
			//{
			//	Tile->RedHighlight();
			//}
			else /*if (bShouldGreenHighlight)*/
			{
				Tile->RedHighlight();
			}
		}
	}
}

void APlayerUnit::EndMouseHoverFocus()
{
	if (!this->bIsCurrentUnit)
	{
		SkeletalMesh->SetRenderCustomDepth(false);

		for (auto Tile : HoverTiles)
		{
			if (Tile.IsValid() && Tile->unitRefrence != this)
			{
				Tile->EndMouseHoverFocus();
			}
		}

		HoverTiles.Empty();
	}
}



void APlayerUnit::Interact(TWeakObjectPtr<APlayerCamera> PlayerCharacter)
{
	if (IsValid(PlayerCharacter.Get()))
	{
	
		this->UpdateInteractableData();

		if(!this->bIsPlayerUnit)
		{
			PlayerAttack(PlayerCharacter);
		}
		
	}
}

void APlayerUnit::Mutate()
{
	if (mutationData)
	{
		mutationData->Mutate();
		if (mutationData->GetLevel() <= unitLevel)
		{
			//UE_LOG(LogTemp, Error, TEXT("Mutation level is higher than unit level"));
			return;
		}
		//pause game only
		if (GameManager.IsValid())
		{
			GameManager->PauseGame();
		}
		++unitLevel;
		//call mutation selection widget
		if (AMainHUD* hud = Cast<AMainHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
		{
			hud->UpdateMutationWidget(GetMutationC1(), GetMutationC2(), GetMutationC3(),this->UnitName, this);
			hud->ShowMutationWidget(); 
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("HUD is null |Mutate_PlayerUnit.cpp|"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("MutationData is null"));
	}
}

TArray<int> APlayerUnit::GetMutationC1()
{
	return mutationData->GetStatsC1();
}

TArray<int> APlayerUnit::GetMutationC2()
{
	return mutationData->GetStatsC2();
}

TArray<int> APlayerUnit::GetMutationC3()
{
	return mutationData->GetStatsC3();
}

float APlayerUnit::GetMouseRotationToEnemy(TWeakObjectPtr<APlayerCamera> Camera)
{
	if (!this)
	{
		UE_LOG(LogTemp, Error, TEXT("GetMouseRotationToEnemy: this (APlayerUnit) is null!"));
		return 0.0f;
	}

	if (!Camera.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("GetMouseRotationToEnemy: Camera is null!"));
		return 0.0f;
	}

	// Get the mouse position on the world grid
	FVector MouseWorldLocation = Camera->GetMouseWorldLocation();
	FVector MouseWorldDirection = Camera->GetMouseWorldDirection();

	// Ensure valid world location and direction
	if (MouseWorldLocation.ContainsNaN() || MouseWorldDirection.ContainsNaN())
	{
		UE_LOG(LogTemp, Error, TEXT("GetMouseRotationToEnemy: Invalid mouse world location or direction!"));
		return 0.0f;
	}

	MouseGridPos = GetMousePosition(MouseWorldLocation, MouseWorldDirection);

	// Validate MouseGridPos values
	if (!FMath::IsFinite(MouseGridPos.X) || !FMath::IsFinite(MouseGridPos.Y))
	{
		UE_LOG(LogTemp, Error, TEXT("GetMouseRotationToEnemy: MouseGridPos contains invalid values!"));
		return 0.0f;
	}

	// Get target location
	FVector TargetLocation = this->GetActorLocation();
	FVector2D AttackDirection = GetCardinalDirection(
		FVector2D(TargetLocation.X, TargetLocation.Y),
		MouseGridPos
	);


	float Angle = 0.0f;

	// Check if the mouse is to the left, right, up, or down of the enemy
	if (AttackDirection.X == 1 && AttackDirection.Y == 0) // Down
	{
		Angle = 0.0f;
	}
	else if (AttackDirection.X == -1 && AttackDirection.Y == 0) // Up
	{
		Angle = 180.0f;
	}
	else if (AttackDirection.Y == 1 && AttackDirection.X == 0) // Left
	{
		Angle = 270.0f;
	}
	else if (AttackDirection.Y == -1 && AttackDirection.X == 0) // Right
	{
		Angle = 90.0f;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GetMouseRotationToEnemy: Unexpected AttackDirection (%f, %f)"),
			AttackDirection.X, AttackDirection.Y);
	}

	// Return the angle
	return Angle;
}


void APlayerUnit::ApplyMutation(TArray<int> statsToAdd)
{
	//speed = 0, damage = 1, defense = 2, health = 3, attack = 4, range = 5, initiative = 6
	if (SB_Mutate) {
		UGameplayStatics::PlaySoundAtLocation(GetWorld(),SB_Mutate, GetActorLocation(), GetActorRotation());
	}
	if (AMainHUD* hud = Cast<AMainHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
	{
		hud->CloseMutationWidget();
	}

	if (statsToAdd.Num() >= 3)
	{

		this->MovementSpeed += statsToAdd[0];
		this->Damage += statsToAdd[1];
		this->Defence += statsToAdd[2];
		this->maxHealth += statsToAdd[3];
		this->Health += statsToAdd[3];
		this->Attack += statsToAdd[4];
		this->AttackRange += statsToAdd[5];
		this->Initiative += statsToAdd[6];


		this->MovementSpeed = (this->MovementSpeed > 0) ? this->MovementSpeed : 1;
		this->Damage = (this->Damage > 0) ? this->Damage : 1;
		this->Defence = (this->Defence > 0) ? this->Defence : 0;
		this->maxHealth = (this->maxHealth > 0) ? this->maxHealth : 1;
		this->Attack = (this->Attack > 0) ? this->Attack : 1;
		this->AttackRange = (this->AttackRange > 0) ? this->AttackRange : 0;
		this->Initiative = (this->Initiative > 0) ? this->Initiative : 1;

	}

	UpdateInteractableData();

	if (GameManager.IsValid())
	{
		GameManager->ResumeGame();
	}
}

void APlayerUnit::Seticon(int num)
{
	UnitIcon = UnitIcons[num];

	//set the material from UnitMaterials to num
	if (UnitMaterials.IsValidIndex(num))
	{
		UMaterialInstanceDynamic* MaterialInstance = UMaterialInstanceDynamic::Create(UnitMaterials[num], this);
		UMaterialInstanceDynamic* MaterialInstanceEye = UMaterialInstanceDynamic::Create(UnitEyeMaterials[num], this);
		if (SkeletalMesh)
		{
			SkeletalMesh->SetMaterial(0, MaterialInstanceEye);
			SkeletalMesh->SetMaterial(1, MaterialInstance);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid material index: %d"), num);
	}
}


void APlayerUnit::UpdateInteractableData()
{
	InstanceInteractableData.InteractableType = EInteractionType::IT_Interact;

	if (bIsPlayerUnit)
	{
		InstanceInteractableData.UnitName = FText::FromString("SwordsMan");
		this->UnitName = FName("PlayerUnit");
	}
	else if (bIsPlayerUnit && bIsRangedUnit) {
		InstanceInteractableData.UnitName = FText::FromString("BowMan");
		this->UnitName = FName("BowMan");
	}
	else
	{
		InstanceInteractableData.UnitName = FText::FromName(UnitName);
	}
	CalculateStats();

	InstanceInteractableData.UnitHealth = Health;
	InstanceInteractableData.maxHealth = maxHealth + MaxHealthFromItems;
	InstanceInteractableData.Range = AttackRange + RangeFromItems;
	InstanceInteractableData.Attack = Attack + AttackFromItems;
	InstanceInteractableData.UnitDamage = Damage;
	InstanceInteractableData.UnitMovementSpeed = FMath::Max(1,MovementSpeed + SpeedFromItems);
	InstanceInteractableData.Defense = Defence + DefenceFromItems;
	InstanceInteractableData.MinDamage = FMath::Max(1, Damage + DamageFromItems - 2);
	InstanceInteractableData.MaxDamage = Damage + DamageFromItems + 2;
	InstanceInteractableData.Initative = Initiative + InitiativeFromItems;

	MinDamage = InstanceInteractableData.MinDamage;

	MaxDamage = InstanceInteractableData.MaxDamage;

}

void APlayerUnit::UpdateHealthBar()
{
	if (!HealthBarWidget.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("HealthBarWidget is NULL! Make sure it's properly set."));
		return;
	}

	float HealthPercent = static_cast<float>(Health) / static_cast<float>(maxHealth + MaxHealthFromItems);
	HealthBarWidget->SetHealthBarPercentage(HealthPercent);

}

void APlayerUnit::UpdateHealthBarRotation()
{

	//set HealthBarWidgetComponent rotation to always face camera/palyerCamera
	if (!HealthBarWidget.IsValid())
		return;

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController) return;

	APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
	if (!CameraManager) return;


	FVector CameraLocation = CameraManager->GetCameraLocation();
	FVector ToCamera = CameraLocation - HealthBarWidgetComponent->GetComponentLocation();
	ToCamera.Normalize();

	FRotator LookAtRotation = ToCamera.Rotation();
	HealthBarWidgetComponent->SetWorldRotation(LookAtRotation);
}

void APlayerUnit::RandomizeStats()
{
	// Randomize the unit's stats
	MovementSpeed = FMath::RandRange(2, 5);
	Damage = FMath::RandRange(1, 5);
	Defence = FMath::RandRange(0, 5);
	maxHealth = FMath::RandRange(8, 15);
	Health = maxHealth;
	Attack = FMath::RandRange(1, 5);
	AttackRange = FMath::RandRange(4, 8);
	Initiative = FMath::RandRange(1, 5);

	// Update the interactable data
	UpdateInteractableData();
}

void APlayerUnit::SetNewPosition(FVector2D NewPosition)
{
	if (!GridManager.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("NO MANAGER |SetNewPosition_PlayerUnit.cpp|"));
		return;
	}

	// Get the old tile and mark it as unoccupied
	AGridTile* OldGridTile = GridManager->GetTileAtPosition(CurrentGridPosition.X, CurrentGridPosition.Y).Get();
	if (OldGridTile)
	{
		OldGridTile->bIsOccupied = false;
		OldGridTile->RemoveUnitReference();
	}

	// Get the new tile and mark it as occupied
	AGridTile* NewGridTile = GridManager->GetTileAtPosition(NewPosition.X, NewPosition.Y).Get();
	if (!NewGridTile)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to cast NewTile to AGridTile |SetNewPosition_PlayerUnit.cpp|"));
		return;
	}

	if (NewGridTile->bIsOccupied)
	{
		UE_LOG(LogTemp, Warning, TEXT("New tile is already occupied |SetNewPosition_PlayerUnit.cpp|"));
		return;
	}

	NewGridTile->bIsOccupied = true;
	NewGridTile->SetUnitReference(this);

	// Update position references
	CurrentGridPosition = NewPosition;

	UE_LOG(LogTemp, Display, TEXT("Position updated to (%f, %f) |SetNewPosition_PlayerUnit.cpp|"), NewPosition.X, NewPosition.Y);
}


