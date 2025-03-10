// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelGenerator.h"
#include "Room.h"
#include "Rat_Conquest/Player/PlayerCamera.h"
#include "Rat_Conquest/Managers/GridManager/GridManager.h"
#include "Rat_Conquest/Managers/GameManager/GameManager.h"
#include "Rat_Conquest/Widgets/MainHUD.h"
#include "Kismet/GameplayStatics.h"


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
ARoom* ALevelGenerator::GetRoomAtPosition(const FVector2D& GridPosition)
{
	for (ARoom* Room : RoomInstances)
	{
		if (Room && Room->GetGridPosition() == GridPosition)
		{
			return Room;
		}
	}
	return nullptr;

}
void ALevelGenerator::OnPlayerEnterRoom(ARoom* _NewRoom)
{
    if (!_NewRoom)
        return;

    // Update the current room
    CurrentRoom = _NewRoom;

    // Find the player camera in the world
    PlayerCamera = nullptr;
    PlayerCamera = Cast<APlayerCamera>(
        UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCamera::StaticClass())
    );
	GridManager = nullptr;
	GridManager = Cast<AGridManager>(   
		UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass())
	);
    // Move the camera to match the room's position
	if (GridManager)
	{   
		FVector NewGridLocation = FVector(CurrentRoom->GetActorLocation().X, CurrentRoom->GetActorLocation().Y, 0);
		//GridManager->SetActorLocation(NewGridLocation);
		GridManager->GridOffset = FVector(CurrentRoom->GetActorLocation().X, CurrentRoom->GetActorLocation().Y, 0);
        GridManager->UpdateGridPosition();
		
	}
    if (PlayerCamera)
    {
		FVector NewCameraLocation = FVector(CurrentRoom->GetActorLocation().X,CurrentRoom->GetActorLocation().Y,PlayerCamera->GetActorLocation().Z);
        PlayerCamera->SetActorLocation(NewCameraLocation);
    }
}
void ALevelGenerator::SetupRoomSelectUI()
{
    if (AMainHUD* hud = Cast<AMainHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
    {
        hud->SetupRoomSelectWidget(this);
        hud->ShowRoomSelectWidget();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("HUD is null |Mutate_PlayerUnit.cpp|"));
    }
}
// Called every frame
void ALevelGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (GetWorld()->GetTimeSeconds() - LastMoveTime < InputCooldown) return;
    if (CurrentRoom)
    {
        // Draw persistent debug sphere over current room
        const FVector RoomLocation = CurrentRoom->GetActorLocation() + FVector(0, 0, 200);
        DrawDebugSphere(
            GetWorld(),
            RoomLocation,
            100.0f,
            12,
            FColor::Yellow,
            false,
            -1.0f,
            0,
            5.0f
        );

    }
    //if (GameManager && !GameManager->bEncounterComplete) return;
    if (GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::Q)) {

        DebugConnectedRooms();
       
    }
    if (GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::Up))
    {
        MoveToRoom(0); // North

    }
    else if (GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::Right))
    {
        MoveToRoom(1); // East
    }
    else if (GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::Down))
    {
        MoveToRoom(2); // South
    }
    else if (GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::Left))
    {
        MoveToRoom(3); // West
    }
   
}

ALevelGenerator::ALevelGenerator()
{
    PrimaryActorTick.bCanEverTick = true;
    RandomStream.GenerateNewSeed();
   
}

void ALevelGenerator::BeginPlay()
{
    Super::BeginPlay();
    TArray<AActor*> FoundGameManagers;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGameManager::StaticClass(), FoundGameManagers);

    if (FoundGameManagers.Num() > 0)
    {
        GameManager = Cast<AGameManager>(FoundGameManagers[0]);
        if (FoundGameManagers.Num() > 1)
        {
            UE_LOG(LogTemp, Warning, TEXT("Multiple GAMEdManagers found! Using first instance."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("No GAMEdManager found in the level!"));
    }
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
                    CurrentRoom = StartRoom;
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
            NeighborDirections[DirIdx] = FMath::RandRange(0,2) == 0;
        }
    }

    return NeighborDirections;
}

void ALevelGenerator::DebugConnectedRooms()
{
    if (!CurrentRoom)
    {
        UE_LOG(LogTemp, Warning, TEXT("No current room selected!"));
        return;
    }

    TArray<ARoom*> ConnectedRooms = FindConnectedRooms(CurrentRoom);
    UE_LOG(LogTemp, Display, TEXT("Found %d connected rooms to current room at (%d,%d)"),
        ConnectedRooms.Num(),
        FMath::RoundToInt(CurrentRoom->GetGridPosition().X),
        FMath::RoundToInt(CurrentRoom->GetGridPosition().Y));

    // Draw debug lines to connected rooms
    const FVector CurrentLocation = CurrentRoom->GetActorLocation() + FVector(0, 0, 100);
    for (ARoom* Room : ConnectedRooms)
    {
        if (Room)
        {
            const FVector TargetLocation = Room->GetActorLocation() + FVector(0, 0, 100);
            DrawDebugLine(
                GetWorld(),
                CurrentLocation,
                TargetLocation,
                FColor::Emerald,
                false,
                5.0f,
                0,
                5.0f
            );
        }
    }
}

TArray<ARoom*> ALevelGenerator::FindConnectedRooms(ARoom* TargetRoom)
{
    TArray<ARoom*> ConnectedRooms;
    if (!TargetRoom) return ConnectedRooms;

    const FVector2D CurrentPos = TargetRoom->GetGridPosition();

    // Check all 4 directions
    for (int32 DirIndex = 0; DirIndex < 4; DirIndex++)
    {
        const FVector2D CheckPos = CurrentPos + GetDirectionVector(DirIndex);

        for (ARoom* Room : RoomInstances)
        {
            if (Room && Room != TargetRoom && Room->GetGridPosition() == CheckPos)
            {
                ConnectedRooms.Add(Room);
            }
        }
    }

    return ConnectedRooms;
}

void ALevelGenerator::MoveToRoom(int32 DirectionIndex)
{
    if (!CurrentRoom || DirectionIndex < 0 || DirectionIndex > 3) return;

    // Get target position
    const FVector2D Direction = GetDirectionVector(DirectionIndex);
    const FVector2D TargetPosition = CurrentRoom->GetGridPosition() + Direction;

    // Find room at target position
    if (ARoom* TargetRoom = GetRoomAtPosition(TargetPosition))
    {
        // Verify door connection (optional)
        if (TargetRoom->GetDoorDirection(GetOppositeDirection(DirectionIndex)))
        {
            CurrentRoom = TargetRoom;
            UE_LOG(LogTemp, Display, TEXT("Moved to room at (%d, %d)"),
                FMath::RoundToInt(TargetPosition.X),
                FMath::RoundToInt(TargetPosition.Y));
            if (AMainHUD* hud = Cast<AMainHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
            {
                hud->SetupRoomSelectWidget(this);
                hud->CloseRoomSelectWidget();
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("HUD is null |Mutate_PlayerUnit.cpp|"));
            }
			OnPlayerEnterRoom(CurrentRoom);
           
            LastMoveTime = GetWorld()->GetTimeSeconds();
            GameManager->LoadNextEncounter();
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("No door connection to room at (%d, %d)"),
                FMath::RoundToInt(TargetPosition.X),
                FMath::RoundToInt(TargetPosition.Y));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No room exists at (%d, %d)"),
            FMath::RoundToInt(TargetPosition.X),
            FMath::RoundToInt(TargetPosition.Y));
    }
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

void ALevelGenerator::GenerateRooms(ARoom* _CurrentRoom)
{
    TArray<bool> Directions;
    _CurrentRoom->GetDoorDirections(Directions);

    for (int32 i = 0; i < Directions.Num(); ++i)
    {
        if (Directions[i] && RoomInstances.Num() < MaxRooms)
        {
            FVector2D NewPosition = _CurrentRoom->GetGridPosition() + GetDirectionVector(i);

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
    if (GameManager) {
        GameManager->bLevelFinishedGenerating = true;
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
