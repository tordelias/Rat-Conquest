// Fill out your copyright notice in the Description page of Project Settings.


#include "GridTile.h"
#include "Rat_Conquest/Player/PlayerCamera.h"
#include "Rat_Conquest/Unit/PlayerUnit.h"

// Sets default values
AGridTile::AGridTile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TileMesh"));
	RootComponent = TileMesh;
	TileMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TileMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	TileMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // Example: only overlap with pawns
	TileMesh->SetGenerateOverlapEvents(true);
	UpdateInteractableData();


}

void AGridTile::BeginFocus()
{
	if (TileMesh)
	{
		TileMesh->SetRenderCustomDepth(true);
	}
}

void AGridTile::EndFocus()
{
	if (TileMesh)
	{
		TileMesh->SetRenderCustomDepth(false);
	}
}

void AGridTile::Interact(APlayerCamera* PlayerCharacter)
{
	if (PlayerCharacter)
	{
		UpdateInteractableData();
		if(PlayerCharacter->GetCurrentUnit())
		{
			PlayerCharacter->GetCurrentUnit()->MoveToTile(GridPosition);
		}
	}
}

void AGridTile::UpdateInteractableData()
{
	InstanceInteractableData.InteractableType = EInteractionType::IT_GridCell;
}

void AGridTile::SetUnitRefrence(APlayerUnit* unit)
{
	unitRefrence = unit;
}

void AGridTile::RemoveUnitRefrence()
{
	unitRefrence = nullptr;
}

void AGridTile::AddOccupant(AActor* tileObj)
{
	if (tileObj && !tileObjects.Contains(tileObj))
	{
		tileObjects.Add(tileObj);
		bIsOccupied = true;
	}
}

void AGridTile::RemoveOccupant(AActor* tileObj)
{

	if (tileObj && tileObjects.Contains(tileObj))
	{
		tileObjects.Remove(tileObj);
		if (tileObjects.Num() == 0)
		{
			bIsOccupied = false;
		}
	}

}

bool AGridTile::IsTileOccupied()
{
	return bIsOccupied;
}

// Called when the game starts or when spawned
void AGridTile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGridTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

