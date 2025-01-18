// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerUnit.h"

//Engine

//includes
#include "Rat_Conquest/GridManager/GridManager.h"
#include "Rat_Conquest/Player/PlayerCamera.h"
#include "Rat_Conquest/GameManager/GameManager.h"
#include "Rat_Conquest/GridTile/GridTile.h"
#include "Kismet/GameplayStatics.h" // Include for UGameplayStatics

// Sets default values
APlayerUnit::APlayerUnit()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	UpdateInteractableData();
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
	AGridTile* GridTile = Cast<AGridTile>(TargetTile);
	if (GridTile)
	{

		if (GridTile->bIsOccupied) {
			return;
		}
		// Example: Set a boolean on the tile
		GridTile->bIsOccupied = true; // Make sure you have this boolean in your AGridTile class
		UE_LOG(LogTemp, Display, TEXT("Target tile is an AGridTile. Setting bIsOccupied to true."));
	}
	else {
		return;
	}
	AActor* OldTile = GridManager->GetTileAt(CurrentGridPosition.X,CurrentGridPosition.Y);
	if (OldTile) {
		AGridTile* OldGridTile = Cast<AGridTile>(OldTile);
		if (OldGridTile)
		{
			// Example: Set a boolean on the tile
			OldGridTile->bIsOccupied = false; // Make sure you have this boolean in your AGridTile class
			UE_LOG(LogTemp, Display, TEXT("Target tile is an AGridTile. Setting bIsOccupied to false."));
		}


	}
		

	SetActorLocation(TargetPosition);

	CurrentGridPosition = NewGridPosition;
	UE_LOG(LogTemp, Error, TEXT("Moving"));

}

void APlayerUnit::ExecutePlayerTurn()
{
	//some logic here
	UE_LOG(LogTemp, Error, TEXT("Player did something"));
	FinishTurn();
}

void APlayerUnit::ExecuteAITurn()
{
	//Some AI logic here
	UE_LOG(LogTemp, Error, TEXT("AI did something"));
	FinishTurn();
}

void APlayerUnit::FinishTurn()
{
	AGameManager* GameManager = Cast<AGameManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameManager::StaticClass()));
	if (GameManager)
	{
		GameManager->EndUnitTurn();
	}
}

void APlayerUnit::BeginFocus()
{
	if (mesh)
	{
		mesh->SetRenderCustomDepth(true);
	}
}

void APlayerUnit::EndFocus()
{
	if (mesh)
	{
		mesh->SetRenderCustomDepth(false);
	}
}

void APlayerUnit::Interact(APlayerCamera* PlayerCharacter)
{
	if (PlayerCharacter)
	{
		// hopefully removed at a later stage 
		PlayerCharacter->SetCurrentUnit(this);
		UpdateInteractableData();
	}
}

// Called when the game starts or when spawned
void APlayerUnit::BeginPlay()
{
	Super::BeginPlay();
	

	// Initialize the player's grid position
	CurrentGridPosition = FVector2D(0, 0); // Start at (0, 0) or any desired position
	MoveToTile(CurrentGridPosition);


}

void APlayerUnit::UpdateInteractableData()
{
	InstanceInteractableData.InteractableType = EInteractionType::IT_Interact;

	//Add more data here
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

