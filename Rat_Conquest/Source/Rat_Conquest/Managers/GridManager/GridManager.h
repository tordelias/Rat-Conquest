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
	void SetGridSize(int32 Rows, int32 Colums);
	void ScanWorldForObjects();
	void UpdateGridPosition();

	AActor* SetStartingPositions(bool _bPlayerUnit);
	AActor* GetClosestAvailableTile(FVector2D Location);

	FVector2D GetGridSize();

	float GetDistanceBetweenTiles(AActor* Tile1, AActor* Tile2);

	AActor* GetTileAt(int32 Row, int32 Column);
	AGridTile* GetTileAtPosition(int32 Row, int32 Column);

	TArray<AGridTile*> GetNeighbourTiles(int32 Row, int32 Column);

	TArray<AGridTile*> GetMovableTiles(int32 Row, int32 Column, int32 MovementRange);

	FVector GetRandomPositionInGrid();

	bool bIsGridFinished() { return GridTiles.Num() > 0; };

	bool bIsGridScanned = false;

	UPROPERTY()
	TMap<FVector2D, AActor*> GridTiles;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//A* pathfinding
	void ResetAllTilesPathfindingData();

	float GridHeight;

	FVector2D WorldToGridPosition(FVector WorldPosition) const;
	FVector GridToWorldPosition(FVector2D GridPosition) const;
	float Tilesize = 100.f; 

	TArray<FVector2D> PlayerPositions;
	TArray<FVector2D> EnemyPositions;
private:
	UPROPERTY(EditAnywhere, Category = "Grid")
	FVector2D GridSize;

	// The class type of the tiles to spawn
	UPROPERTY(EditAnywhere, Category = "Grid")
	TSubclassOf<AActor> GridTileClass;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	FVector GridOffset = FVector::ZeroVector; // World-space offset
};
