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
	bool& direction = DoorDirections[dirIndex];
	return direction;
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

    int BridgeLength = 5; // Adjust to control bridge length
    int InitialOffsetX = 4.25; // How many tiles away the bridge starts on the X axis
    int InitialOffsetY = 6; // How many tiles away the bridge starts on the Y axis

    // Offsets for moving directionally
    FVector DirectionOffsets[4] = {
        FVector(0, -1, 0),  // North
        FVector(1, 0, 0),   // East
        FVector(0, 1, 0),   // South
        FVector(-1, 0, 0)   // West
    };

    // Initial offset based on direction (applied before bridge extension)
    FVector InitialOffsets[4] = {
        FVector(0, -InitialOffsetY, 0),  // North: Offset on Y
        FVector(InitialOffsetX, 0, 0),   // East: Offset on X
        FVector(0, InitialOffsetY, 0),   // South: Offset on Y
        FVector(-InitialOffsetX, 0, 0)   // West: Offset on X
    };

    for (int i = 0; i < 4; ++i)
    {
        if (DoorDirections[i])
        {
            // Start further away using the appropriate initial offset
            FVector Location = GetActorLocation() + (InitialOffsets[i] * 100);
            Location.Z = 50.f; // Ensure Z remains the same

            for (int j = 0; j < BridgeLength; ++j)
            {
                // Extend the bridge in the direction
                Location += (DirectionOffsets[i] * 100);

                // Spawn the tile
                AActor* Tile = GetWorld()->SpawnActor<AActor>(GridTileClass, Location, FRotator::ZeroRotator, SpawnParams);
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

