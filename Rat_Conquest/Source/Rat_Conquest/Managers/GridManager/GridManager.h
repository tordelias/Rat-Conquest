// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridManager.generated.h"

class AGridTile;
class AStaticMeshActor;
class AItem;

UCLASS()
class RAT_CONQUEST_API AGridManager : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AGridManager();

    void GenerateGrid(int32 Rows, int32 Columns, float TileSize);
    void GetCenterTile(int32 Row, int32 Column);
    void SetGridSize(int32 Rows, int32 Columns);
    void ScanWorldForObjects();
    void UpdateGridPosition();
    void ClearUnitsFromTile();

    TWeakObjectPtr<AActor> SetStartingPositions(bool bPlayerUnit);
    TWeakObjectPtr<AActor> GetClosestAvailableTile(FVector2D Location);

    FVector2D GetGridSize() const { return GridSize; }

    float GetDistanceBetweenTiles(const TWeakObjectPtr<AActor>& Tile1, const TWeakObjectPtr<AActor>& Tile2);

    TWeakObjectPtr<AActor> GetTileAt(int32 Row, int32 Column);
    TWeakObjectPtr<AGridTile> GetTileAtPosition(int32 Row, int32 Column);

    TArray<TWeakObjectPtr<AGridTile>> GetNeighbourTiles(int32 Row, int32 Column);
    TArray<TWeakObjectPtr<AGridTile>> GetMovableTiles(int32 Row, int32 Column, int32 MovementRange);

    FVector GetRandomPositionInGrid();

    bool IsGridFinished() const { return GridTiles.Num() > 0; }
    bool IsGridScanned() const { return bIsGridScanned; }

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // A* pathfinding
    void ResetAllTilesPathfindingData();

    float GridHeight;

    FVector2D WorldToGridPosition(FVector WorldPosition) const;
    FVector GridToWorldPosition(FVector2D GridPosition) const;

    UPROPERTY(EditAnywhere, Category = "Grid")
    float TileSize = 100.f;

    UPROPERTY()
    TMap<FVector2D, TWeakObjectPtr<AActor>> GridTiles;

    TArray<FVector2D> PlayerPositions;
    TArray<FVector2D> EnemyPositions;

    UPROPERTY()
    bool bIsGridScanned = false;

    UPROPERTY(EditAnywhere, Category = "Grid")
    FVector2D GridSize;
private:

    // The class type of the tiles to spawn
    UPROPERTY(EditAnywhere, Category = "Grid")
    TSubclassOf<AActor> GridTileClass;

    
	float Tsize;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
    FVector GridOffset = FVector::ZeroVector; // World-space offset
};