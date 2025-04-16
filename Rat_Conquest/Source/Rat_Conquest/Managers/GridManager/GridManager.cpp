#include "GridManager.h"
#include "Rat_Conquest/GridTile/GridTile.h"
#include "Rat_Conquest/Items/Item.h"
#include "Rat_Conquest/LevelObjects/InteractableGridObject.h"

// Sets default values
AGridManager::AGridManager()
{
	// Set this actor to call Tick() every frame. Turn this off for better performance if not needed.
	PrimaryActorTick.bCanEverTick = true;
    SetGridSize(10, 12);

	GridHeight = GetActorLocation().Z;
}

// Generate the grid of tiles
void AGridManager::GenerateGrid(int32 Rows, int32 Columns, float Size)
{
    GridTiles.Empty(); // Clear any existing tiles

	this->Tsize = Size;
    // Check if the GridTileClass is set
    if (GridTileClass == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("GridTileClass is not set!"));
        return;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // Calculate offsets to center the grid
    int32 RowOffset = (Rows - 1) / 2;
    int32 ColumnOffset = (Columns - 1) / 2;

    // Get the starting position from the actor's location
    FVector StartingPosition = GetActorLocation();

    // Loop through rows and columns to create the grid
    for (int32 i = 0; i < Rows; i++)
    {
        for (int32 j = 0; j < Columns; j++)
        {
            // Calculate world position (centered around actor's location)
            float XPosition = StartingPosition.X + (i - RowOffset) * Size;
            float YPosition = StartingPosition.Y + (j - ColumnOffset) * Size;
            FVector Location(XPosition, YPosition, StartingPosition.Z);

            // Spawn the tile
            AActor* Tile = GetWorld()->SpawnActor<AActor>(GridTileClass, Location, FRotator::ZeroRotator, SpawnParams);
            if (Tile != nullptr)
            {

               
                // Logical position with (0,0) as bottom-left corner
                FVector2D LogicalGridPosition(i, j);

                // Map to bottom-left corner logical indexing
                LogicalGridPosition.Y = j;    // Columns stay as-is
                LogicalGridPosition.X = Rows - 1 - i; // Reverse row index for bottom-left origin

                // Store in the map
                GridTiles.Add(LogicalGridPosition, Tile);

                // Set GridPosition on the tile
                AGridTile* GridTile = Cast<AGridTile>(Tile);
                if (GridTile != nullptr)
                {
                    GridTile->GridPosition = LogicalGridPosition;
                    UE_LOG(LogTemp, Display, TEXT("Spawned tile at LogicalGridPosition: (%f, %f)"), LogicalGridPosition.X, LogicalGridPosition.Y);
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("Failed to cast spawned tile to AGridTile at (%d, %d)"), i, j);
                }
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to spawn tile at (%d, %d)"), i, j);
            }
        }
    }
}


void AGridManager::GetCenterTile(int32 Row, int32 Column)
{
    if (Row <= 0 || Column <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid grid dimensions! Rows: %d, Columns: %d"), Row, Column);
        return;
    }

    
}

void AGridManager::SetGridSize(int32 Rows, int32 Colums)
{
    GridSize.X = Rows;
    GridSize.Y = Colums;

}

void AGridManager::ScanWorldForObjects()
{
    int num = 0;
    PlayerPositions.Empty();
    EnemyPositions.Empty();
    bIsGridScanned = false;
    for (auto& TilePair : GridTiles)
    {
        AGridTile* Tile = Cast<AGridTile>(TilePair.Value);
        if (Tile) {
			if (!Tile->CheckIfTileOccupied())
			{
				Tile->bIsOccupied = false;
            }
         

        }
			
       
    }
    for (auto& TilePair : GridTiles)
    {
        AGridTile* Tile = Cast<AGridTile>(TilePair.Value);
        if (!Tile)
            continue;
        if (Tile->bIsOccupied)
            num++;
        // Clear existing occupants
        Tile->TileObjects.Empty();
		Tile->ItemSlot = nullptr;
        
        // Check for overlapping actors
        TArray<AActor*> OverlappingActors;
        Tile->TileMesh->GetOverlappingActors(OverlappingActors); // Assumes TileMesh is set to query overlaps
        
        for (AActor* Actor : OverlappingActors)
        {
            // Check if Actor is a GridTile and matches testTile
            AGridTile* OverlappingTile = Cast<AGridTile>(Actor);
            UStaticMeshComponent* StaticMeshComp = Actor->FindComponentByClass<UStaticMeshComponent>();
            AItem* Item = Cast<AItem>(Actor);
			AInteractableGridObject* InteractableObj = Cast<AInteractableGridObject>(Actor);

            if (StaticMeshComp && Actor->ActorHasTag(TEXT("GridObj")))
            {
                // The overlapping actor has a StaticMeshComponent
                Tile->AddOccupant(Actor);
                UE_LOG(LogTemp, Display, TEXT("Found actor with StaticMesh at Row: %f, Column: %f, Name: %s"), Tile->GridPosition.X, Tile->GridPosition.Y, *Actor->GetName());
                
            }
			if (StaticMeshComp && Item)
			{
				
				Tile->ItemSlot = Item;
				UE_LOG(LogTemp, Error, TEXT("Found item at Row: %f, Column: %f"), Tile->GridPosition.X, Tile->GridPosition.Y);
			}
            if (StaticMeshComp && Actor->ActorHasTag(TEXT("PlayerPos"))) {
				PlayerPositions.Add(FVector2D(Tile->GridPosition.X, Tile->GridPosition.Y));

                UE_LOG(LogTemp, Error, TEXT("FOUND PLAYER POS at Row:"));

            }
            if (StaticMeshComp && Actor->ActorHasTag(TEXT("EnemyPos"))) {
                EnemyPositions.Add(FVector2D(Tile->GridPosition.X, Tile->GridPosition.Y));
 
                UE_LOG(LogTemp, Error, TEXT("FOUND ENEMY POS at Row:"));

            }
			if (StaticMeshComp && Actor->ActorHasTag(TEXT("InteractObj")) && InteractableObj)
			{
                Tile->InteractableObjectSlot = InteractableObj;
                UE_LOG(LogTemp, Display, TEXT("Found interactable object at Row: %f, Column: %f"), Tile->GridPosition.X, Tile->GridPosition.Y);
			}

        }
    }
	UE_LOG(LogTemp, Error, TEXT("Found %d occupied tiles in the grid"), num);
    bIsGridScanned = true;

}

void AGridManager::UpdateGridPosition()
{
    const int32 Rows = GridSize.X;
    const int32 Columns = GridSize.Y;
    const FVector NewOrigin = GetActorLocation() + GridOffset;

    // Recalculate offsets for centering
    const int32 RowOffset = (Rows - 1) / 2;
    const int32 ColumnOffset = (Columns - 1) / 2;

    for (const auto& TilePair : GridTiles)
    {
        AActor* Tile = TilePair.Value.Get();
        if (!Tile) continue;

        // Get original logical position from map key
        const FVector2D LogicalPos = TilePair.Key;

        // Reverse the logical X position calculation
        int32 OriginalRowIndex = Rows - 1 - LogicalPos.X;
        int32 OriginalColumnIndex = LogicalPos.Y;

        // Calculate new world position
        float XPos = NewOrigin.X + (OriginalRowIndex - RowOffset) * Tsize;
        float YPos = NewOrigin.Y + (OriginalColumnIndex - ColumnOffset) * Tsize;
        FVector NewLocation(XPos, YPos, NewOrigin.Z);

        // Update tile position
        Tile->SetActorLocation(NewLocation);

        // Optional: Update debug visualization
        if (AGridTile* GridTile = Cast<AGridTile>(Tile))
        {
            DrawDebugBox(
                GetWorld(),
                NewLocation,
                FVector(Tsize / 2, Tsize / 2, 10),
                FColor::Yellow,
                true,
                -1.0f,
                0,
                2.0f
            );
        }
    }

    UE_LOG(LogTemp, Display, TEXT("Grid moved to new origin: %s"), *NewOrigin.ToString());
}

void AGridManager::ClearUnitsFromTile()
{
    TArray<AGridTile*> TileCopy;
    for (auto& TilePair : GridTiles)
    {
        AGridTile* Tile = Cast<AGridTile>(TilePair.Value);
        if (Tile)
        {
            TileCopy.Add(Tile);
        }
    }

    for (AGridTile* Tile : TileCopy)
    {
        if (Tile)
        {
            Tile->ItemSlot = nullptr;
            Tile->bIsOccupied = false;
            Tile->TileObjects.Empty();
            Tile->SetUnitReference(nullptr);
        }
    }
	
}

TWeakObjectPtr<AActor> AGridManager::SetStartingPositions(bool _bPlayerUnit)
{
    if (_bPlayerUnit) {

		for (int i = 0; i < PlayerPositions.Num(); ++i)
		{
			FVector2D GridPosition = PlayerPositions[i];
			AGridTile* Tile = GetTileAtPosition(GridPosition.X, GridPosition.Y).Get();
			if (Tile && !Tile->bIsOccupied)
			{
				Tile->bIsOccupied = true;
                UE_LOG(LogTemp, Display, TEXT("Set player unit at Row: %f, Column: %f"), Tile->GridPosition.X, Tile->GridPosition.Y);
                return Tile;
                
				
			}
		}
        
    }
    else {

        for (int i = 0; i < EnemyPositions.Num(); ++i)
        {
            FVector2D GridPosition = EnemyPositions[i];
            AGridTile* Tile = GetTileAtPosition(GridPosition.X, GridPosition.Y).Get();
            if (Tile && !Tile->bIsOccupied)
            {
                Tile->bIsOccupied = true;
                UE_LOG(LogTemp, Display, TEXT("Set enemy unit at Row: %f, Column: %f"), Tile->GridPosition.X, Tile->GridPosition.Y);
                return Tile;
            }
        }
    }
    return nullptr;
	
}

TWeakObjectPtr<AActor> AGridManager::GetClosestAvailableTile(FVector2D Location)
{
    AActor* ClosestTile = nullptr;
    float MinDistance = FLT_MAX;

    for (auto& TilePair : GridTiles)
    {
		// Check if the tile is valid and not occupied
        AGridTile* Tile = Cast<AGridTile>(TilePair.Value);
        if (Tile && !Tile->bIsOccupied) // Check if the tile is valid and not occupied
        {
            FVector2D TileLocation(Tile->GridPosition.X, Tile->GridPosition.Y);
            float Distance = FVector2D::Distance(Location, TileLocation);

            if (Distance < MinDistance)
            {
                MinDistance = Distance;
                ClosestTile = Tile;
            }
        }
    }

    if (ClosestTile)
    {
        AGridTile* Tile = Cast<AGridTile>(ClosestTile);
		UE_LOG(LogTemp, Display, TEXT("Found closest tile at Row: %f, Column: %f"), Tile->GridPosition.X, Tile->GridPosition.Y);
        return ClosestTile;
    }

	// If no available tile is found, return an nullptr
    return nullptr;
}

float AGridManager::GetDistanceBetweenTiles(const TWeakObjectPtr<AActor>& Tile1, const TWeakObjectPtr<AActor>& Tile2)
{
    if (!Tile1.IsValid() || !Tile2.IsValid())
        return 9999;

    AGridTile* GridTile1 = Cast<AGridTile>(Tile1);
    AGridTile* GridTile2 = Cast<AGridTile>(Tile2);
    if (!GridTile1 || !GridTile2)
        return 9999;

    FVector2D tile1Pos = GridTile1->GridPosition;
    FVector2D tile2Pos = GridTile2->GridPosition;

    float Distance = FVector2D::Distance(tile1Pos, tile2Pos);
    UE_LOG(LogTemp, Display, TEXT("Distance between tiles: %f"), Distance);
    return Distance;
}


// Get the tile at a specific grid coordinate
TWeakObjectPtr<AActor> AGridManager::GetTileAt(int32 Row, int32 Column)
{
    FVector2D GridPosition(Row, Column);

    if (GridTiles.Num() < 0) return nullptr;
    if (GridTiles.Contains(GridPosition))
    {
        AActor* Tile = GridTiles[GridPosition].Get();
        //UE_LOG(LogTemp, Display, TEXT("Found tile at Row: %d, Column: %d"), Row, Column);
        return Tile;
    }
    else
    {
       // UE_LOG(LogTemp, Warning, TEXT("No tile found at Row: %d, Column: %d"), Row, Column);
        return nullptr;
    }

}
TWeakObjectPtr<AGridTile> AGridManager::GetTileAtPosition(int32 Row, int32 Column)
{
    FVector2D GridPosition(Row, Column);

    if (GridTiles.Contains(GridPosition))
    {
        AActor* Tile = GridTiles[GridPosition].Get();
		//cast to GridTile
		AGridTile* GridTile = Cast<AGridTile>(Tile);
        return GridTile;
    }
    else
    {
        // UE_LOG(LogTemp, Warning, TEXT("No tile found at Row: %d, Column: %d"), Row, Column);
        return nullptr;
    }

}

TArray<TWeakObjectPtr<AGridTile>> AGridManager::GetNeighbourTiles(int32 Row, int32 Column)
{
    TArray<TWeakObjectPtr<AGridTile>> NeighbourTiles;
    if (Row < 0 || Column < 0 || GridSize == FVector2D::ZeroVector)
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid grid dimensions! Row: %d, Column: %d"), Row, Column);
        return NeighbourTiles;
    }

    // Check four orthogonal directions instead of all 8 neighbors
    const TArray<FIntPoint> Directions = {
        FIntPoint(1, 0),   // Right
        FIntPoint(-1, 0),  // Left
        FIntPoint(0, 1),    // Up
        FIntPoint(0, -1)    // Down
    };

    for (const FIntPoint& Dir : Directions)
    {
        int32 i = Row + Dir.X;
        int32 j = Column + Dir.Y;

        // Check bounds
        if (i >= 0 && i < GridSize.X && j >= 0 && j < GridSize.Y)
        {
            AGridTile* Tile = Cast<AGridTile>(GetTileAt(i, j));
            if (Tile != nullptr)
            {
                NeighbourTiles.Add(Tile);
            }
        }
    }

    return NeighbourTiles;
}

TArray<TWeakObjectPtr<AGridTile>> AGridManager::GetMovableTiles(int32 Row, int32 Column, int32 MovementRange)
{
    TArray<TWeakObjectPtr<AGridTile>> MovableTiles;
    TArray<AGridTile*> OpenList;
    TArray<AGridTile*> ClosedList;

    ResetAllTilesPathfindingData();

    AGridTile* StartTile = Cast<AGridTile>(GetTileAt(Row, Column).Get());
    if (!StartTile)
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid start tile at Row: %d, Column: %d"), Row, Column);
        return MovableTiles;
    }

    // Initialize start tile
    StartTile->G = 0;
    OpenList.Add(StartTile);

    while (OpenList.Num() > 0)
    {
        // Get tile with lowest G cost (closest to start)
        AGridTile* CurrentTile = OpenList[0];
        for (AGridTile* Tile : OpenList)
        {
            if (Tile->G < CurrentTile->G)
            {
                CurrentTile = Tile;
            }
        }

        // Move from open to closed list
        OpenList.Remove(CurrentTile);
        ClosedList.Add(CurrentTile);

        // Get neighbors (same as your pathfinding)
        TArray<AGridTile*> NeighbourTiles;
        NeighbourTiles.Add(GetTileAtPosition(CurrentTile->GridPosition.X + 1, CurrentTile->GridPosition.Y).Get()); // Right
        NeighbourTiles.Add(GetTileAtPosition(CurrentTile->GridPosition.X - 1, CurrentTile->GridPosition.Y).Get()); // Left
        NeighbourTiles.Add(GetTileAtPosition(CurrentTile->GridPosition.X, CurrentTile->GridPosition.Y + 1).Get()); // Up
        NeighbourTiles.Add(GetTileAtPosition(CurrentTile->GridPosition.X, CurrentTile->GridPosition.Y - 1).Get()); // Down

        for (AGridTile* Neighbour : NeighbourTiles)
        {
            if (!Neighbour || ClosedList.Contains(Neighbour))
            {
                continue;
            }

            // Calculate movement cost (using Chebyshev distance like your pathfinding)
            float MoveCost = FMath::Max(
                FMath::Abs(CurrentTile->GridPosition.X - Neighbour->GridPosition.X),
                FMath::Abs(CurrentTile->GridPosition.Y - Neighbour->GridPosition.Y)
            );
            float TentativeG = CurrentTile->G + MoveCost;

            // Skip if beyond movement range or blocked
            if (TentativeG > MovementRange || Neighbour->bIsOccupied)
            {
                continue;
            }

            // If we found a better path to this neighbor
            if (TentativeG < Neighbour->G || !OpenList.Contains(Neighbour))
            {
                Neighbour->G = TentativeG;

                if (!OpenList.Contains(Neighbour))
                {
                    OpenList.Add(Neighbour);
                }
            }
        }
    }

    // Convert closed list to result (excluding start tile and blocked tiles)
    for (AGridTile* Tile : ClosedList)
    {
        if (Tile != StartTile && !Tile->bIsOccupied)
        {
            MovableTiles.Add(Tile);
        }
    }

    return MovableTiles;
}


FVector AGridManager::GetRandomPositionInGrid()
{
    if (GridTiles.Num() == 0)
    {
        return FVector(); // Return default if no tiles exist
    }

    // Collect all unoccupied tiles
    TArray<AActor*> AvailableTiles;
    for (const auto& TilePair : GridTiles)
    {
      
        AGridTile* CurrentTile = Cast<AGridTile>(TilePair.Value);
        if (CurrentTile && !CurrentTile->bIsOccupied)  // Ensure the tile exists and is not occupied
        {
            AvailableTiles.Add(CurrentTile);
        }
    }

    if (AvailableTiles.Num() == 0)
    {
        return FVector(); // No unoccupied tiles available
    }

    // Pick a random unoccupied tile
    int32 RandomIndex = FMath::RandRange(0, AvailableTiles.Num() - 1);
    return AvailableTiles[RandomIndex]->GetActorLocation();
}



// Called when the game starts or when spawned
void AGridManager::BeginPlay()
{
	Super::BeginPlay();
    
	GenerateGrid(GridSize.X, GridSize.Y, 100.0f);
	//GetTileAt(3, 2);
	//GetCenterTile(Rows, Columns);
    //GetNeighbourTiles(2, 1,Rows,Columns);
    //GetDistanceBetweenTiles(GetTileAt(2, 3), GetTileAt(3, 1));
	//ScanWorldForObjects();
    
}

// Called every frame
void AGridManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    if (GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::T))
    {
        //ScanWorldForObjects();
		ClearUnitsFromTile();
		UE_LOG(LogTemp, Display, TEXT("Scanning for objects!"));

    }
}

void AGridManager::ResetAllTilesPathfindingData()
{
    for (auto& Tilemap : GridTiles) 
    {
		AGridTile* Tile = Cast<AGridTile>(Tilemap.Value);
        if (Tile)
        {
            Tile->ResetPathfindingData();
        }
    }
}

FVector2D AGridManager::WorldToGridPosition(FVector WorldPosition) const
{
    return FVector2D(
        (WorldPosition.X - this->GetActorLocation().X) / Tsize,
        (WorldPosition.Y - this->GetActorLocation().Y) / Tsize
    );
}
FVector AGridManager::GridToWorldPosition(FVector2D GridPosition) const
{
    return FVector(
        GridPosition.X * Tsize + this->GetActorLocation().X,
        GridPosition.Y * Tsize + this->GetActorLocation().Y,
        GridHeight
    );
}
