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

	UpdateInteractableData();

	TileMesh->SetRenderCustomDepth(true);

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
	//if (TileMesh)
	//{
	//	TileMesh->SetRenderCustomDepth(false);
	//}
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

