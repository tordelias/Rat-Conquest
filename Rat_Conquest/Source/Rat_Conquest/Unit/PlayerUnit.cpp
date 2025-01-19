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

// Called when the game starts or when spawned
void APlayerUnit::BeginPlay()
{
	Super::BeginPlay();

	//if (bIsPlayerUnit) {
	//	DelayedInitalPosition();
	//}
	DelayedInitalPosition();
}
// Called every frame
void APlayerUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsMoving)
	{
		MovementProgress += DeltaTime / MovementDuration;

		if (MovementProgress >= 1.0f)
		{
			MovementProgress = 1.0f;
			bIsMoving = false;
			SetActorLocation(TargetPosition);
			UE_LOG(LogTemp, Display, TEXT("Finished moving to new tile: %s"), *TargetPosition.ToString());
		}
		else
		{
			FVector NewPosition = FMath::Lerp(StartPosition, TargetPosition, MovementProgress);
			SetActorLocation(NewPosition);
		}
	}
	if (GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::E))
	{
		TArray<AGridTile*> listofTiles = GridManager->GetNeighbourTiles(CurrentGridPosition.X,CurrentGridPosition.Y);


	}
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

	AGridTile* GridTile = Cast<AGridTile>(TargetTile);
	if (!GridTile || GridTile->bIsOccupied)
		return;

	AActor* OldTile = GridManager->GetTileAt(CurrentGridPosition.X, CurrentGridPosition.Y);
	if (OldTile) {
		AGridTile* OldGridTile = Cast<AGridTile>(OldTile);
		if (OldGridTile) {
			OldGridTile->bIsOccupied = false;
			OldGridTile->RemoveUnitRefrence();
		}
			
	}
	

	if (GridManager->GetDistanceBetweenTiles(TargetTile, OldTile) > movementSpeed)
		return;

	
	// Mark new tile as occupied
	GridTile->bIsOccupied = true;
	GridTile->SetUnitRefrence(this);
	// Set up lerp variables
	StartPosition = GetActorLocation();
	TargetPosition = TargetTile->GetActorLocation();
	MovementProgress = 0.0f;  // Reset progress
	bIsMoving = true;         // Start movement

	CurrentGridPosition = NewGridPosition;
	UE_LOG(LogTemp, Display, TEXT("Started moving to new tile."));
	
}

void APlayerUnit::SetInitalPosition(FVector2D position)
{
	if (!GridManager) {
		UE_LOG(LogTemp, Error, TEXT("NO MANAGER"));
		return;
	}

	AActor* TargetTile = GridManager->GetTileAt(position.X, position.Y);
	if (!TargetTile) {
		UE_LOG(LogTemp, Warning, TEXT("Invalid target tile at (%f, %f)"), position.X, position.Y);
		return;
	}

	TargetPosition = TargetTile->GetActorLocation();
	SetActorLocation(TargetPosition);
	CurrentGridPosition = position;
	UE_LOG(LogTemp, Display, TEXT("Player unit set to grid position (%f, %f)"), position.X, position.Y);
}

void APlayerUnit::DelayedInitalPosition()
{
	if (GridManager && GridManager->bIsGridFinished())
	{
		SetInitalPosition(FVector2D(0,0));
	}
	else
	{
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimerForNextTick(this, &APlayerUnit::DelayedInitalPosition);
	}
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



void APlayerUnit::UpdateInteractableData()
{
	InstanceInteractableData.InteractableType = EInteractionType::IT_Interact;

	//Add more data here
}


