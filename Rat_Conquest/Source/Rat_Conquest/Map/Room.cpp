// Fill out your copyright notice in the Description page of Project Settings.


#include "Room.h"

// Sets default values
ARoom::ARoom()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RoomMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RoomMesh"));
	RootComponent = RoomMesh;
}
bool& ARoom::GetDoorDirection(int32 dirIndex)
{
    static bool defaultFalse = false; 

    if (dirIndex < 0 || dirIndex >= DoorDirections.Num()) 
    {
        return defaultFalse; 
    }

    return DoorDirections[dirIndex]; 
}

TArray<bool> ARoom::GetDoorDirections(TArray<bool>)
{
	return DoorDirections;
}

int32 ARoom::GetDoorCount() const
{
	int32 Count = 0;
	for (bool bDirection : DoorDirections)
	{
		if (bDirection) Count++;
	}
	return Count;
}

void ARoom::DrawDebugDoors()
{
  /*  const FVector Location = GetActorLocation();
    const FRotator Rotation = GetActorRotation();

    for (int32 i = 0; i < DoorDirections.Num(); i++)
    {
        if (DoorDirections[i])
        {
            const FVector Offset = FVector(
                GetDirectionVector(i).X * 500,
                GetDirectionVector(i).Y * 500,
                0
            );

            DrawDebugDirectionalArrow(
                GetWorld(),
                Location,
                Location + Offset,
                100.f,
                FColor::Emerald,
                true,
                -1.f,
                0,
                10.f
            );
        }
    }*/
}

void ARoom::ConstructBridges()
{
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    const int32 BridgeLength = 5; // Adjust to control bridge length
    const int32 InitialOffsetX = 1; // Tiles away on X axis
    const int32 InitialOffsetY = 1; // Tiles away on Y axis

    // Configuration structure for direction parameters
    struct FDirectionConfig
    {
        float OffsetMultiplier;
        FVector2D PositionAdjustments;
        bool bAdjustX;
    };

    // Direction order: North, East, South, West
    const FDirectionConfig DirectionConfigs[4] = {
        {525.0f, FVector2D(25.0f, -75.0f), true},   // North
        {425.0f, FVector2D(-25.0f, 75.0f), false},  // East
        {575.0f, FVector2D(25.0f, -75.0f), true},   // South
        {475.0f, FVector2D(-25.0f, 75.0f), false}   // West
    };

    const FVector DirectionVectors[4] = {
        FVector(0, -1, 0),  // North
        FVector(1, 0, 0),   // East
        FVector(0, 1, 0),   // South
        FVector(-1, 0, 0)   // West
    };

    const FVector InitialOffsets[4] = {
        FVector(0, -InitialOffsetY, 0),  // North
        FVector(InitialOffsetX, 0, 0),   // East
        FVector(0, InitialOffsetY, 0),   // South
        FVector(-InitialOffsetX, 0, 0)   // West
    };

    for (int32 i = 0; i < 4; ++i)
    {
        if (!DoorDirections[i]) {
            continue;
        }
        const FDirectionConfig& Config = DirectionConfigs[i];
        const FVector BaseOffset = InitialOffsets[i] * Config.OffsetMultiplier;
        const FVector BaseLocation = GetActorLocation() + BaseOffset;

        // Create two parallel bridge starting points
        for (const float Adjustment : {Config.PositionAdjustments.X, Config.PositionAdjustments.Y})
        {
            FVector SpawnLocation = BaseLocation;
            SpawnLocation.Z = GetActorLocation().Z - 25;

            // Apply position adjustment
            if (Config.bAdjustX) {
                SpawnLocation.X += Adjustment;
            }
            else {
                SpawnLocation.Y += Adjustment;
            }

            // Spawn bridge tiles
            for (int32 j = 0; j < BridgeLength; ++j)
            {
                SpawnLocation += DirectionVectors[i] * 100;
                GetWorld()->SpawnActor<AActor>(GridTileClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
            }
        }
    }
}


// Called when the game starts or when spawned
void ARoom::BeginPlay()
{
	Super::BeginPlay();
    ConstructBridges();
	
}

// Called every frame
void ARoom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

