#include "GridManager.h"
#include "Rat_Conquest/GridTile/GridTile.h"

// Sets default values
AGridManager::AGridManager()
{
	// Set this actor to call Tick() every frame. Turn this off for better performance if not needed.
	PrimaryActorTick.bCanEverTick = true;
}

// Generate the grid of tiles
void AGridManager::GenerateGrid(int32 Rows, int32 Columns, float TileSize)
{
    GridTiles.Empty(); // Clear any existing tiles

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
    int32 RowOffset;
    if (Rows % 2 == 0)
    {
        RowOffset = Rows / 2;
    }
    else
    {
        RowOffset = (Rows / 2) + 1;
    }

    int32 ColumnOffset;
    if (Columns % 2 == 0)
    {
        ColumnOffset = Columns / 2;
    }
    else
    {
        ColumnOffset = (Columns / 2) + 1;
    }

    // Loop through rows and columns to create the grid
    for (int32 i = 0; i < Rows; i++)
    {
        for (int32 j = 0; j < Columns; j++)
        {
            float XPosition = (i - RowOffset + 0.5f) * TileSize;
            float YPosition = (j - ColumnOffset + 0.5f) * TileSize;
            FVector Location(XPosition, YPosition, 0.0f);

            // Spawn the tile
            AActor* Tile = GetWorld()->SpawnActor<AActor>(GridTileClass, Location, FRotator::ZeroRotator, SpawnParams);
            if (Tile != nullptr)
            {
                FVector2D GridPosition(i, j);
                GridTiles.Add(GridPosition, Tile);

                AGridTile* GridTile = Cast<AGridTile>(Tile);
                if (GridTile != nullptr)
                {
                    GridTile->GridPosition = GridPosition;
                    UE_LOG(LogTemp, Display, TEXT("Spawned tile at (%d, %d)"), i, j);
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


// Get the tile at a specific grid coordinate
AActor* AGridManager::GetTileAt(int32 Row, int32 Column)
{
    FVector2D GridPosition(Row, Column);

    if (GridTiles.Contains(GridPosition))
    {
        AActor* Tile = GridTiles[GridPosition];
        UE_LOG(LogTemp, Display, TEXT("Found tile at Row: %d, Column: %d"), Row, Column);
        return Tile;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No tile found at Row: %d, Column: %d"), Row, Column);
        return nullptr;
    }

}

// Called when the game starts or when spawned
void AGridManager::BeginPlay()
{
	Super::BeginPlay();
	int32 Rows = 10;
	int32 Columns = 12;

	GenerateGrid(Rows, Columns, 100.0f);
	//GetTileAt(3, 2);
	//GetCenterTile(Rows, Columns);
}

// Called every frame
void AGridManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
