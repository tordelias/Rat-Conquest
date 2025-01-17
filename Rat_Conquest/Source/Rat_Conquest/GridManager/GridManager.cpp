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
    GridTiles.SetNum(Rows * Columns);

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    for (int32 i = 0; i < Rows; i++)
    {
        for (int32 j = 0; j < Columns; j++)
        {
            FVector Location = FVector(i * TileSize, j * TileSize, 0.0f);

            if (GridTileClass)
            {
                AActor* Tile = GetWorld()->SpawnActor<AActor>(GridTileClass, Location, FRotator::ZeroRotator, SpawnParams);
                if (Tile)
                {
                    int32 Index = i * Columns + j;
                    GridTiles[Index] = Tile;
                    AGridTile* GridTile = Cast<AGridTile>(Tile);
                    if (GridTile)
                    {
                        GridTile->GridPosition = FVector2D(i, j);
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
            else
            {
                UE_LOG(LogTemp, Error, TEXT("GridTileClass is not set!"));
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

    int32 CenterRow = Row / 2;
    int32 CenterColumn = Column / 2;

    int32 CenterIndex = CenterRow * Column + CenterColumn;
    if (GridTiles.IsValidIndex(CenterIndex))
    {
        AActor* CenterTile = GridTiles[CenterIndex];
        UE_LOG(LogTemp, Display, TEXT("Center tile is at Row: %d, Column: %d (Index: %d)"), CenterRow, CenterColumn, CenterIndex);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Center tile index is invalid!"));
    }
}


// Get the tile at a specific grid coordinate
AActor* AGridManager::GetTileAt(int32 Row, int32 Column)
{
	if (Row < 0 || Column < 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid grid coordinates! Row: %d, Column: %d"), Row, Column);
		return nullptr;
	}

	int32 Index = Row * Column + Column;
	if (GridTiles.IsValidIndex(Index))
	{
		AActor* Tile = GridTiles[Index];
		UE_LOG(LogTemp, Display, TEXT("Found tile at Row: %d, Column: %d (Index: %d)"), Row, Column, Index);
		return Tile;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Tile index is invalid!"));
		return nullptr;
	}

}

// Called when the game starts or when spawned
void AGridManager::BeginPlay()
{
	Super::BeginPlay();
	int32 Rows = 8;
	int32 Columns = 8;

	GenerateGrid(Rows, Columns, 100.0f);
	GetTileAt(3, 2);
	GetCenterTile(Rows, Columns);
}

// Called every frame
void AGridManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
