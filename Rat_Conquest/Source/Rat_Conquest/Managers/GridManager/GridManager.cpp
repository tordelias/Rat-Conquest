#include "GridManager.h"
#include "Rat_Conquest/GridTile/GridTile.h"
#include "Rat_Conquest/Items/Item.h"

// Sets default values
AGridManager::AGridManager()
{
	// Set this actor to call Tick() every frame. Turn this off for better performance if not needed.
	PrimaryActorTick.bCanEverTick = true;
    SetGridSize(10, 12);

	GridHeight = GetActorLocation().Z;
}

// Generate the grid of tiles
void AGridManager::GenerateGrid(int32 Rows, int32 Columns, float TileSize)
{
    GridTiles.Empty(); // Clear any existing tiles

	this->Tilesize = TileSize;
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
            float XPosition = StartingPosition.X + (i - RowOffset) * TileSize;
            float YPosition = StartingPosition.Y + (j - ColumnOffset) * TileSize;
            FVector Location(XPosition, YPosition, StartingPosition.Z);

            // Spawn the tile
            AActor* Tile = GetWorld()->SpawnActor<AActor>(GridTileClass, Location, FRotator::ZeroRotator, SpawnParams);
            if (Tile != nullptr)
            {

                FVector TileCenter = Tile->GetActorLocation();
                FVector BoxExtent(TileSize / 2, TileSize / 2, 10.0f); // Adjust Z height as needed
                FColor DebugColor = FColor::Yellow;

                // Draw debug box around the tile
                DrawDebugBox(
                    GetWorld(),
                    TileCenter,
                    BoxExtent,
                    FQuat::Identity,
                    DebugColor,
                    true,       
                    -1.0f,       
                    0,           
                    2.0f         // Thickness
                );
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
    for (auto& TilePair : GridTiles)
    {
        AGridTile* Tile = Cast<AGridTile>(TilePair.Value);
        if (!Tile)
            continue;
        if (Tile->bIsOccupied)
            num++;
        // Clear existing occupants
        Tile->tileObjects.Empty();
        
        // Check for overlapping actors
        TArray<AActor*> OverlappingActors;
        Tile->TileMesh->GetOverlappingActors(OverlappingActors); // Assumes TileMesh is set to query overlaps
        
        for (AActor* Actor : OverlappingActors)
        {
            // Check if Actor is a GridTile and matches testTile
            AGridTile* OverlappingTile = Cast<AGridTile>(Actor);
            UStaticMeshComponent* StaticMeshComp = Actor->FindComponentByClass<UStaticMeshComponent>();
            AItem* Item = Cast<AItem>(Actor);
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
                UE_LOG(LogTemp, Error, TEXT("FOUND PLAYER POS at Row:"));

            }

        }
    }
	UE_LOG(LogTemp, Error, TEXT("Found %d occupied tiles in the grid"), num);

}

AActor* AGridManager::GetClosestAvailableTile(FVector2D Location)
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

FVector2D AGridManager::GetGridSize()
{
    return GridSize;
}

float AGridManager::GetDistanceBetweenTiles(AActor* Tile1, AActor* Tile2)
{
    if (!Tile1 || !Tile2)
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
AActor* AGridManager::GetTileAt(int32 Row, int32 Column)
{
    FVector2D GridPosition(Row, Column);

    if (GridTiles.Contains(GridPosition))
    {
        AActor* Tile = GridTiles[GridPosition];
        //UE_LOG(LogTemp, Display, TEXT("Found tile at Row: %d, Column: %d"), Row, Column);
        return Tile;
    }
    else
    {
       // UE_LOG(LogTemp, Warning, TEXT("No tile found at Row: %d, Column: %d"), Row, Column);
        return nullptr;
    }

}
AGridTile* AGridManager::GetTileAtPosition(int32 Row, int32 Column)
{
    FVector2D GridPosition(Row, Column);

    if (GridTiles.Contains(GridPosition))
    {
        AActor* Tile = GridTiles[GridPosition];
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

TArray<AGridTile*> AGridManager::GetNeighbourTiles(int32 Row, int32 Column)
{
    TArray<AGridTile*> NeighbourTiles;
    if (Row < 0 || Column < 0 || GridSize == FVector2D::ZeroVector)
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid grid dimensions! Row: %d, Column: %d"), Row, Column);
        return NeighbourTiles;
    }

    int32 StartRow = FMath::Max(0, Row - 1);
    int32 EndRow = FMath::Min(Row + 1, GridSize.X - 1);
    int32 StartColumn = FMath::Max(0, Column - 1);
    int32 EndColumn = FMath::Min(Column + 1, GridSize.Y - 1);

    for (int32 i = StartRow; i <= EndRow; ++i)
    {
        for (int32 j = StartColumn; j <= EndColumn; ++j)
        {
            if (i == Row && j == Column)
            {
                continue; // Skip the center tile
            }

            AGridTile* Tile = Cast<AGridTile>(GetTileAt(i, j));
            if (Tile != nullptr)
            {
                NeighbourTiles.Add(Tile);
            }
        }
    }
    return NeighbourTiles;
}

TArray<AGridTile*> AGridManager::GetMovableTiles(int32 Row, int32 Column, int32 MovementRange)
{
    TSet<AGridTile*> VisitedTiles; // Use a TSet to prevent duplicate tiles
    TQueue<FIntPoint> TilesToVisit; // Use a queue for breadth-first search (BFS)

    // Add the starting tile
    AActor* StartingTileActor = GetTileAt(Row, Column);
    AGridTile* StartingTile = Cast<AGridTile>(StartingTileActor);
    if (StartingTile)
    {
        TilesToVisit.Enqueue(FIntPoint(Row, Column));
        VisitedTiles.Add(StartingTile);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Starting tile is not valid at Row: %d, Column: %d"), Row, Column);
        return TArray<AGridTile*>();
    }

    while (!TilesToVisit.IsEmpty())
    {
        FIntPoint Current = FIntPoint::ZeroValue;
        TilesToVisit.Dequeue(Current);

        int32 CurrentRow = Current.X;
        int32 CurrentColumn = Current.Y;

        // Get neighbors of the current tile
        TArray<AGridTile*> Neighbors = GetNeighbourTiles(CurrentRow, CurrentColumn);

        for (AGridTile* Neighbor : Neighbors)
        {
            if (!Neighbor || Neighbor->bIsOccupied) continue;
            int32 NeighborRow = Neighbor->GridPosition.X;
            int32 NeighborColumn = Neighbor->GridPosition.Y;

            int32 RowDiff = FMath::Abs(NeighborRow - Row);
            int32 ColDiff = FMath::Abs(NeighborColumn - Column);
            int32 Distance = FMath::Max(RowDiff, ColDiff);

            if (Distance <= MovementRange && !VisitedTiles.Contains(Neighbor))
            {
                VisitedTiles.Add(Neighbor);
                TilesToVisit.Enqueue(FIntPoint(NeighborRow, NeighborColumn));
            }
        }
    }

    // Convert TSet to TArray for return
    return VisitedTiles.Array();
}


// Called when the game starts or when spawned
void AGridManager::BeginPlay()
{
	Super::BeginPlay();
    
	GenerateGrid(GridSize.X, GridSize.Y, 100.0f);
	//GetTileAt(3, 2);
	//GetCenterTile(Rows, Columns);
    //GetNeighbourTiles(2, 1,Rows,Columns);
    GetDistanceBetweenTiles(GetTileAt(2, 3), GetTileAt(3, 1));
	ScanWorldForObjects();
    
}

// Called every frame
void AGridManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    if (GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::T))
    {
        ScanWorldForObjects();
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
        (WorldPosition.X - this->GetActorLocation().X) / Tilesize,
        (WorldPosition.Y - this->GetActorLocation().Y) / Tilesize
    );
}
FVector AGridManager::GridToWorldPosition(FVector2D GridPosition) const
{
    return FVector(
        GridPosition.X * Tilesize + this->GetActorLocation().X,
        GridPosition.Y * Tilesize + this->GetActorLocation().Y,
        GridHeight
    );
}
