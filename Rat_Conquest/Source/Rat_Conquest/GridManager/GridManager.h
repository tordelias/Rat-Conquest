// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridManager.generated.h"


class AGridTile;
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
	FVector2D GetGridSize();

	float GetDistanceBetweenTiles(AActor* Tile1, AActor* Tile2);

	AActor* GetTileAt(int32 Row, int32 Column);

	TArray<AGridTile*> GetNeighbourTiles(int32 Row, int32 Column);

	bool bIsGridFinished() { return GridTiles.Num() > 0; };

	UPROPERTY()
	TMap<FVector2D, AActor*> GridTiles;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
private:

	FVector2D GridSize;

	// The class type of the tiles to spawn
	UPROPERTY(EditAnywhere, Category = "Grid")
	TSubclassOf<AActor> GridTileClass;
};
