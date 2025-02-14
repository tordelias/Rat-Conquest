// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelGenerator.h"
#include "Room.h"



// Called when the game starts or when spawned

void ALevelGenerator::DrawDebugGrid()
{
    for (const FVector2D& GridPos : GridPositions)
    {
        const FVector WorldPos = FVector(GridPos.X * RoomSize.X, GridPos.Y * RoomSize.Y, 0);
        DrawDebugBox(GetWorld(), WorldPos, FVector(RoomSize.X / 2, RoomSize.Y / 2, 50),
            FColor::Green, true, -1, 0, 5);
    }
}
// Called every frame
void ALevelGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
  
}

ALevelGenerator::ALevelGenerator()
{
    PrimaryActorTick.bCanEverTick = true;
    RandomStream.GenerateNewSeed();
}

void ALevelGenerator::BeginPlay()
{
    Super::BeginPlay();
    GenerateInitialRooms();
    DrawDebugGrid();
}

void ALevelGenerator::GenerateInitialRooms()
{
    StartPosition = FVector2D::ZeroVector;
    const int32 MaxAttempts = 100;
    int32 Attempts = 0;
    bool bFoundValidRoom = false;

    while (Attempts < MaxAttempts && !bFoundValidRoom)
    {
        // Shuffle templates every 5 attempts
        if (Attempts % 5 == 0)
        {
            ShuffleRoomTemplates();
        }

        for (TSubclassOf<ARoom> RoomClass : RoomTemplates)
        {
            // Clear previous invalid position
            GridPositions.Remove(StartPosition);

            if (ARoom* StartRoom = SpawnRoom(StartPosition, RoomClass))
            {
                if (StartRoom->GetDoorCount() >= 3)
                {
                    UE_LOG(LogTemp, Warning, TEXT("Found valid starting room"));
                    PutRoomInList(StartRoom, StartPosition);
                    GenerateRooms(StartRoom);
                    bFoundValidRoom = true;
                    return;
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("Invalid starting room - not enough doors"));
                    GridPositions.Remove(StartPosition); // Critical: Remove invalid position
                    StartRoom->Destroy();
                }
            }
        }
        Attempts++;
    }

    UE_LOG(LogTemp, Error, TEXT("Failed to find valid starting room after %d attempts"), MaxAttempts);
  
}

ARoom* ALevelGenerator::SpawnRoom(const FVector2D& GridPosition, TSubclassOf<ARoom> RoomClass)
{
    if (!IsPositionValid(GridPosition)) return nullptr;

    FVector WorldPosition = FVector(GridPosition.X * RoomSize.X, GridPosition.Y * RoomSize.Y, 0);
    ARoom* NewRoom = GetWorld()->SpawnActor<ARoom>(RoomClass, WorldPosition, FRotator::ZeroRotator);
    if (NewRoom)
    {
        NewRoom->SetGridPosition(GridPosition);
        GridPositions.Add(GridPosition);
        RoomInstances.Add(NewRoom);
    }
    return NewRoom;
}

bool ALevelGenerator::CheckValidPositionForBigRooms(const FVector2D& GridPosition, int32 RoomTemplateIndex, const TArray<bool>& RequiredDirections)
{
  
    return true;
}

bool ALevelGenerator::IsPositionValid(const FVector2D& Position) const
{
    return !GridPositions.Contains(Position);
}

FVector2D ALevelGenerator::GetDirectionVector(int32 DirectionIndex) const
{
    const FVector2D Directions[] = {
        FVector2D(0, -1),  // North
        FVector2D(1, 0),   // East
        FVector2D(0, 1),   // South
        FVector2D(-1, 0)   // West
    };
    return Directions[DirectionIndex];
}

void ALevelGenerator::PutRoomInList(ARoom* Room, const FVector2D& GridPosition)
{
    if (!Room || !IsPositionValid(GridPosition)) return;

    GridPositions.Add(GridPosition);
    RoomInstances.Add(Room);

    //// Handle big rooms
    //if (Room->IsBigRoom())
    //{
    //    const FVector2D RoomSize = Room->GetRoomSize();
    //    for (int32 X = 0; X < RoomSize.X; X++)
    //    {
    //        for (int32 Y = 0; Y < RoomSize.Y; Y++)
    //        {
    //            if (X == 0 && Y == 0) continue;
    //            GridPositions.Add(GridPosition + FVector2D(X, Y));
    //        }
    //    }
    //}
}

int32 ALevelGenerator::GetOppositeDirection(int32 DirectionIndex)
{
    return (DirectionIndex + 2) % 4;
}

TArray<bool> ALevelGenerator::CheckNeighbors(const FVector2D& GridPosition, int32 DirectionIndex)
{
    TArray<bool> NeighborDirections;
    NeighborDirections.Init(false, 4);

    for (int32 DirIdx = 0; DirIdx < 4; DirIdx++)
    {
        const FVector2D CheckPos = GridPosition + GetDirectionVector(DirIdx);

        if (!IsPositionValid(CheckPos))
        {
            for (ARoom* ExistingRoom : RoomInstances)
            {
                if (ExistingRoom->GetGridPosition() == CheckPos)
                {
                    
                    if (ExistingRoom->GetDoorDirection(GetOppositeDirection(DirIdx)) == true) {

                        NeighborDirections[DirIdx] = true;

                       
                    }
                    else {
                        NeighborDirections[DirIdx] = false;
                    }
                    
                }
            }
        }
        else
        {
            // Random chance for potential door
            NeighborDirections[DirIdx] = FMath::RandRange(0,4) == 0;
        }
    }

    return NeighborDirections;
}

void ALevelGenerator::ShuffleRoomTemplates()
{
    for (int32 i = RoomTemplates.Num() - 1; i > 0; i--)
    {
        const int32 j = RandomStream.RandRange(0, i);
        RoomTemplates.Swap(i, j);
        UE_LOG(LogTemp, Warning, TEXT("Shuffle!"));
    }
}

int32 ALevelGenerator::FindRoomWithCorrectDoors(const TArray<bool>& RequiredDirections)
{
    TArray<int32> PotentialRooms;
    bool bMatches = false;
    for (int32 i = 0; i < RoomTemplates.Num(); i++)
    {
        if (ARoom* DefaultRoom = Cast<ARoom>(RoomTemplates[i]->GetDefaultObject()))
        {
           
            for (int32 DirIdx = 0; DirIdx < 4; DirIdx++)
            {
                if (DefaultRoom->GetDoorDirection(DirIdx) != RequiredDirections[DirIdx])
                {
                    bMatches = false;
                    break;
                }
                else {
                    bMatches = true;
                }
            }
            if (bMatches) PotentialRooms.Add(i);
        }
    }

    return PotentialRooms.Num() > 0 ?
        PotentialRooms[RandomStream.RandRange(0, PotentialRooms.Num() - 1)] :
        INDEX_NONE;
}

void ALevelGenerator::RegenerateRooms()
{
    for (ARoom* Room : RoomInstances)
    {
        if (IsValid(Room))
        {
            Room->Destroy();
        }
    }
    RoomInstances.Empty();
    GridPositions.Empty();
    GenerateInitialRooms();
}

void ALevelGenerator::GenerateRooms(ARoom* CurrentRoom)
{
    TArray<bool> Directions;
    CurrentRoom->GetDoorDirections(Directions);

    for (int32 i = 0; i < Directions.Num(); ++i)
    {
        if (Directions[i] && RoomInstances.Num() < MaxRooms)
        {
            FVector2D NewPosition = CurrentRoom->GetGridPosition() + GetDirectionVector(i);

            if (IsPositionValid(NewPosition))
            {
                int32 RandomIndex = RandomStream.RandRange(0, RoomTemplates.Num() - 1);
                ARoom* NewRoom = SpawnRoom(NewPosition, RoomTemplates[RandomIndex]);
                if (NewRoom) GenerateRooms(NewRoom);
            }
        }
    }
    for (int i = 0; i < FMath::RandRange(InitialRooms, MaxRooms); ++i) {

        CheckOpenDoors();
    }
    if (RoomInstances.Num() >= MaxRooms)
    {
        //PlaceEndRooms();
    }
}

void ALevelGenerator::CheckOpenDoors()
{
    for (ARoom* Room : RoomInstances)
    {
        if (!Room)
            continue;
        TArray<bool> Directions;
        Directions = Room->GetDoorDirections(Directions);

        for (int32 DirIndex = 0; DirIndex < 4; DirIndex++)
        {
            if (Directions[DirIndex])
            {
                const FVector2D NewPos = Room->GetGridPosition() + GetDirectionVector(DirIndex);

                if (IsPositionValid(NewPos))
                {
                    TArray<bool> NeighborDirections = CheckNeighbors(NewPos, DirIndex);
                    const int32 RoomIndex = FindRoomWithCorrectDoors(NeighborDirections);

                    if (RoomIndex != INDEX_NONE)
                    {
                        SpawnRoom(NewPos, RoomTemplates[RoomIndex]);
                        
                        
                           
                        
                    }
                }
            }
        }
    }
}

void ALevelGenerator::PlaceEndRooms()
{
    // Implementation for placing end rooms similar to Godot logic
  
}

void ALevelGenerator::EnsureAllDoorsFilled()
{

}

void ALevelGenerator::PlaceBigRoomPieces(ARoom* MainRoom, const FVector2D& MainGridPosition)
{

}
