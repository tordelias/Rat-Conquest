// Fill out your copyright notice in the Description page of Project Settings.

#include "GridTile.h"
#include "Rat_Conquest/Player/PlayerCamera.h"
#include "Rat_Conquest/Unit/PlayerUnit.h"
#include "Rat_Conquest/Items/Item.h"

// Sets default values
AGridTile::AGridTile()
{
    PrimaryActorTick.bCanEverTick = true;

    // Create the static mesh component and set it as the root
    TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TileMesh"));
    RootComponent = TileMesh;

    // Set collision settings
    TileMesh->SetCollisionResponseToAllChannels(ECR_Ignore); // Ignore everything by default
    TileMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // Overlap with pawns
    TileMesh->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap); // Overlap with dynamic objects
    TileMesh->SetGenerateOverlapEvents(true);

    // Initialize visibility and render settings
    TileMesh->SetVisibility(false);
    TileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    //This is for Highlighting
    TileMesh->SetRenderCustomDepth(false);
    TileMesh->SetCustomDepthStencilValue(0);

    // Initialize variables
    bIsHighlightedByUnit = false;
    bIsHighlightedByGameManager = false;

    UpdateInteractableData();
}

//make focus color dependent
void AGridTile::BeginFocus()
{
    //Highlight Collors Green == 1, Red == 2, Yellow == 3
    if (!bIsHighlightedByGameManager) // Do not override GameManager highlights
    {
        bIsHighlightedByUnit = true;
    }
}

void AGridTile::EndFocus()
{
    if (bIsHighlightedByUnit)
    {
        bIsHighlightedByUnit = false;
		bIsGreenHighlighted = false;
		bIsRedHighlighted = false;
		this->EndHighlight();
    }
}

void AGridTile::GreenHighlight()
{
    if (bIsBridgeTile) {
        return;
    }
    
    //Highlight Collors Green == 1, Red == 2, Yellow == 3
	if (!bIsHighlightedByUnit)
	{
		TileMesh->SetRenderCustomDepth(true);
		TileMesh->SetVisibility(true);
		TileMesh->SetCustomDepthStencilValue(1);
        bIsGreenHighlighted = true;
	}
}

void AGridTile::RedHighlight()
{

    if (bIsBridgeTile) {
        return;
    }
    //Highlight Collors Green == 1, Red == 2, Yellow == 3
    if(!bIsHighlightedByUnit)
	{
		TileMesh->SetRenderCustomDepth(true);
		TileMesh->SetVisibility(true);
		TileMesh->SetCustomDepthStencilValue(2);
		bIsRedHighlighted = true;
	}
}

void AGridTile::YellowHighlight()
{
    	//Highlight Collors Green == 1, Red == 2, Yellow == 3
	if (bIsRedHighlighted || bIsGreenHighlighted)
	{
		TileMesh->SetCustomDepthStencilValue(3);
	}
}

void AGridTile::EndHighlight()
{
    if (bIsBridgeTile) {
        return;
    }

    if (!bIsHighlightedByUnit)
	{
        if(!bIsGreenHighlighted && !bIsRedHighlighted)
        {
            TileMesh->SetRenderCustomDepth(false);
            TileMesh->SetVisibility(false);
            TileMesh->SetCustomDepthStencilValue(0);
            bIsHighlightedByUnit = false;
            bIsGreenHighlighted = false;
            bIsRedHighlighted = false;
        }
	}
}

void AGridTile::BeginMouseHoverFocus()
{
    //Highlight Collors Green == 1, Red == 2, Yellow == 3
    if (bIsBridgeTile) {
        return;
    }
    if (!bIsHighlightedByUnit) 
    {
        TileMesh->SetVisibility(true);
        TileMesh->SetRenderCustomDepth(true);
        TileMesh->SetCustomDepthStencilValue(2);
    }
    else
    {
		this->YellowHighlight();
    }
    if (IsValid(unitRefrence.Get()))
    {
		unitRefrence->BeginMouseHoverFocus();
    }
}

void AGridTile::EndMouseHoverFocus()
{
    if (bIsBridgeTile) {
        return;
    }

    if (!bIsHighlightedByUnit)
    {
        TileMesh->SetRenderCustomDepth(false);
        TileMesh->SetVisibility(false);
        TileMesh->SetCustomDepthStencilValue(0);
        bIsRedHighlighted = false;
        bIsGreenHighlighted = false;
    }

    if (bIsGreenHighlighted)
    {
        TileMesh->SetCustomDepthStencilValue(1);
    }
    else if (bIsRedHighlighted)
    {
        TileMesh->SetCustomDepthStencilValue(2);
    }

    if (IsValid(unitRefrence.Get()) && unitRefrence->bIsCurrentUnit == false)
    {
        unitRefrence->EndMouseHoverFocus();
    }
}


void AGridTile::Interact(APlayerCamera* PlayerCharacter)
{
    if (PlayerCharacter)
    {
        UpdateInteractableData();
        if (PlayerCharacter->GetCurrentUnit().Get() && !bIsOccupied)
        {
            PlayerCharacter->GetCurrentUnit()->MoveToTile(GridPosition);
            
        }
        if (bIsOccupied && unitRefrence != nullptr)
        {
            if (!unitRefrence->bIsPlayerUnit)
            {
				unitRefrence->PlayerAttack(PlayerCharacter);

            }

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

void AGridTile::ResetPathfindingData()
{
    G = 0.f;
    H = 0.f;
    F = 0.f;
}
