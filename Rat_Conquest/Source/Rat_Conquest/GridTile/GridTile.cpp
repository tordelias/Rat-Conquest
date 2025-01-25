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
    TileMesh->SetRenderCustomDepth(false);

    // Initialize variables
    bIsHighlightedByUnit = false;
    bIsHighlightedByGameManager = false;

    UpdateInteractableData();
}


void AGridTile::BeginFocus()
{
    if (!bIsHighlightedByGameManager) // Do not override GameManager highlights
    {
        TileMesh->SetRenderCustomDepth(true);
        TileMesh->SetVisibility(true);
        bIsHighlightedByUnit = true;
    }
}

void AGridTile::EndFocus()
{
    if (bIsHighlightedByUnit) // Only affect unit-specific highlighting
    {
        TileMesh->SetRenderCustomDepth(false);
        TileMesh->SetVisibility(false);
        bIsHighlightedByUnit = false;
    }
}

void AGridTile::BeginMouseHoverFocus()
{
    if (!bIsHighlightedByGameManager && !bIsHighlightedByUnit) // Avoid double-highlighting
    {
        TileMesh->SetVisibility(true);
        TileMesh->SetRenderCustomDepth(true);
    }
}

void AGridTile::EndMouseHoverFocus()
{
    if (!bIsHighlightedByUnit && !bIsHighlightedByGameManager) // Preserve GameManager highlights
    {
        TileMesh->SetRenderCustomDepth(false);
        TileMesh->SetVisibility(false);
    }
}

void AGridTile::SetGameManagerHighlight()
{
    if (!bIsHighlightedByGameManager)
    {
        TileMesh->SetRenderCustomDepth(true);
        bIsHighlightedByGameManager = true;
    }
}

void AGridTile::ClearGameManagerHighlight()
{
    if (bIsHighlightedByGameManager)
    {
        TileMesh->SetRenderCustomDepth(false);
        bIsHighlightedByGameManager = false;
    }
}

void AGridTile::Interact(APlayerCamera* PlayerCharacter)
{
    if (PlayerCharacter)
    {
        UpdateInteractableData();
        if (PlayerCharacter->GetCurrentUnit())
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

void AGridTile::ResetPathfindingData()
{
    G = 0.f;
    H = 0.f;
    F = 0.f;
}
