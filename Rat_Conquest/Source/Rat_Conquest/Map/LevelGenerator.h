// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelGenerator.generated.h"



class ARoom;
class APlayerCamera;
class AGridManager;
class AGameManager;
UCLASS()
class RAT_CONQUEST_API ALevelGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, Category = "Room Settings")
	TArray<TSubclassOf<ARoom>> RoomTemplates;

	UPROPERTY(EditAnywhere, Category = "Room Settings")
	TArray<ARoom*> EndRoomTemplates;

	UPROPERTY(EditAnywhere, Category = "Grid Settings")
	FVector2D RoomSize = FVector2D(1408, 768);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings")
	int32 InitialRooms = 3;
	UPROPERTY(EditAnywhere, Category = "Grid Settings")
	int32 MaxRooms = 4;



	float InputCooldown = 0.5f;
	float LastMoveTime = -1.0f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	TArray<FVector2D> GridPositions;
	TArray<ARoom*> RoomInstances;
	FVector2D StartPosition = FVector2D::ZeroVector;
	int32 CurrentRooms = 0;
	FRandomStream RandomStream;

	ARoom* CurrentRoom = nullptr;
	APlayerCamera* PlayerCamera = nullptr;
	AGridManager* GridManager = nullptr;
	AGameManager* GameManager = nullptr;

	void GenerateInitialRooms();
	void GenerateRooms(ARoom* CurrentRoom);
	void CheckOpenDoors();
	bool IsPositionValid(const FVector2D& Position) const;
	ARoom* SpawnRoom(const FVector2D& GridPosition, TSubclassOf<ARoom> RoomClass);
	bool CheckValidPositionForBigRooms(const FVector2D& GridPosition, int32 RoomTemplateIndex, const TArray<bool>& RequiredDirections);
	void PlaceEndRooms();
	void EnsureAllDoorsFilled();
	void PlaceBigRoomPieces(ARoom* MainRoom, const FVector2D& MainGridPosition);
	FVector2D GetDirectionVector(int32 DirectionIndex) const;
	int32 FindRoomWithCorrectDoors(const TArray<bool>& RequiredDirections);
	void RegenerateRooms();
	void PutRoomInList(ARoom* Room, const FVector2D& GridPosition);
	int32 GetOppositeDirection(int32 DirectionIndex);
	TArray<bool> CheckNeighbors(const FVector2D& GridPosition, int32 DirectionIndex);
	void DebugConnectedRooms();
	TArray<ARoom*> FindConnectedRooms(ARoom* TargetRoom);
	void MoveToRoom(int32 DirectionIndex);
	void ShuffleRoomTemplates();

	void DrawDebugGrid();
	ARoom* GetRoomAtPosition(const FVector2D& GridPosition);

	void OnPlayerEnterRoom(ARoom* _NewRoom);
	void SetupRoomSelectUI();
};
