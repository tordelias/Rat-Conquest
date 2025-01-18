// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerUnit.h"

// Sets default values
APlayerUnit::APlayerUnit()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void APlayerUnit::MoveToTile(FVector2D NewGridPosition)
{
	if (!GridManager) {
		UE_LOG(LogTemp, Error, TEXT("NO MANAGER"));
		return;
	}

	AActor* TargetTile = GridManager->GetTileAt(NewGridPosition.X, NewGridPosition.Y);
	if (!TargetTile)
		return;

	FVector TargetPosition = TargetTile->GetActorLocation();

	SetActorLocation(TargetPosition);

	CurrentGridPosition = NewGridPosition;
	UE_LOG(LogTemp, Error, TEXT("Moving"));

}

// Called when the game starts or when spawned
void APlayerUnit::BeginPlay()
{
	Super::BeginPlay();
	

	// Initialize the player's grid position
	CurrentGridPosition = FVector2D(0, 0); // Start at (0, 0) or any desired position
	MoveToTile(CurrentGridPosition);


}

// Called every frame
void APlayerUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::E))
	{
		MoveToTile(FVector2D(1, 0));

		
	}
	
}

