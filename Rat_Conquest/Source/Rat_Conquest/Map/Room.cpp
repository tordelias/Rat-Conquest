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

// Called when the game starts or when spawned
void ARoom::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARoom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

