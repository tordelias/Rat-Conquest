// Fill out your copyright notice in the Description page of Project Settings.

#include "GameManager.h"
#include "Rat_Conquest/Unit/PlayerUnit.h"
#include "Rat_Conquest/Player/PlayerCamera.h"
#include "Kismet/GameplayStatics.h"
#include "Rat_Conquest/Managers/GridManager/GridManager.h"
#include "Rat_Conquest/GridTile/GridTile.h"
#include "EngineUtils.h"

// Sets default values
AGameManager::AGameManager()
{
    // Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
}

void AGameManager::TogglePlayerTurn()
{
    bisPlayersturn = true;
}

void AGameManager::InitalizeUnits()
{
    for (TActorIterator<APlayerUnit> It(GetWorld()); It; ++It)
    {
        APlayerUnit* Unit = *It;
        if (Unit->bIsPlayerUnit)
        {
            PlayerUnits.Add(Unit);
            UE_LOG(LogTemp, Warning, TEXT("Player unit added"));
        }
        else
        {
            EnemyUnits.Add(Unit);
            UE_LOG(LogTemp, Warning, TEXT("Enemy unit added"));
        }
    }
}

void AGameManager::StartTurnOrder()
{
    TurnQueue.Empty();
   
    if (bisPlayersturn)
    {
        if (PlayerUnits.Num() > 0)
        {
            TurnQueue.Append(PlayerUnits);
        }
        if (EnemyUnits.Num() > 0)
        {
            TurnQueue.Append(EnemyUnits);
        }
    }
    else
    {
        if (EnemyUnits.Num() > 0)
        {
            TurnQueue.Append(EnemyUnits);
        }
        if (PlayerUnits.Num() > 0)
        {
            TurnQueue.Append(PlayerUnits);
        }
    }

    if (TurnQueue.Num() > 0)
    {
        CurrentUnit = TurnQueue[0];
        HighlightUnitAndTiles(CurrentUnit);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("TurnQueue is empty! No unit to execute turn."));
        return;
    }
    UE_LOG(LogTemp, Warning, TEXT("Size of TurnQueue: %d"), TurnQueue.Num());
    ExecuteTurn();
}

void AGameManager::ExecuteTurn()
{
    if (!CurrentUnit)
        return;

    if (CurrentUnit->bIsPlayerUnit)
    {
        APlayerCamera* PlayerCharacter = Cast<APlayerCamera>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
        if (PlayerCharacter)
        {
            PlayerCharacter->SetCurrentUnit(CurrentUnit);
        }
        CurrentUnit->ExecutePlayerTurn();
		UE_LOG(LogTemp, Warning, TEXT("Player unit turn executed"));
    }
    else
    {
        CurrentUnit->ExecuteAITurn();
    }
}

void AGameManager::EndUnitTurn()
{
    if (PlayerUnits.Num() == 0 || EnemyUnits.Num() == 0)
    {
		UE_LOG(LogTemp, Warning, TEXT("No more units to take turn!"));
        return;
    }

    if (TurnQueue.Num() == 0)
    {
        
        UE_LOG(LogTemp, Warning, TEXT("TurnQueue is empty! No unit to execute turn."));
		return;
    }

    TurnQueue.RemoveAt(0);

    if (TurnQueue.Num() > 0)
    {
        CurrentUnit = TurnQueue[0];
        HighlightUnitAndTiles(CurrentUnit);
        ExecuteTurn();
    }
    else
    {
        bisPlayersturn = !bisPlayersturn;
        StartTurnOrder();
    }
   
}

void AGameManager::RemoveUnitFromQueue(APlayerUnit* unit)
{
    if (!unit)
    {
        UE_LOG(LogTemp, Error, TEXT("RemoveUnitFromQueue failed: Unit is null!"));
        return;
    }

    // Remove the unit from the turn queue
    TurnQueue.Remove(unit);

    // Remove the unit from PlayerUnits or EnemyUnits
    if (unit->bIsPlayerUnit)
    {
        PlayerUnits.Remove(unit);
    }
    else
    {
        EnemyUnits.Remove(unit);
    }

    // Log for debugging
    UE_LOG(LogTemp, Display, TEXT("Unit removed from queue and lists."));
}

void AGameManager::HighlightUnitAndTiles(APlayerUnit* NewUnit)
{
    // Unhighlight the previous unit and its tiles
    if (CurrentlyFocusedUnit)
    {
        CurrentlyFocusedUnit->EndFocus();

        for (AGridTile* Tile : CurrentlyFocusedTiles)
        {
            if (Tile)
            {
                Tile->EndFocus();
            }
        }
        CurrentlyFocusedTiles.Empty();
    }

    // Highlight the new unit
    if (NewUnit)
    {
        CurrentlyFocusedUnit = NewUnit;
        CurrentlyFocusedUnit->BeginFocus();

        // Highlight movable tiles
        AGridManager* GridManager = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass()));
        if (GridManager)
        {
            TArray<AGridTile*> MovableTiles = GridManager->GetMovableTiles(
                NewUnit->CurrentGridPosition.X, NewUnit->CurrentGridPosition.Y, NewUnit->MovementSpeed);

            for (AGridTile* Tile : MovableTiles)
            {
                if (Tile)
                {
                    Tile->BeginFocus();
                    CurrentlyFocusedTiles.Add(Tile);
                }
            }
        }
    }
    else
    {
        CurrentlyFocusedUnit = nullptr;
    }
}

// Called when the game starts or when spawned
void AGameManager::BeginPlay()
{
    Super::BeginPlay();
    InitalizeUnits();
    TogglePlayerTurn();
    StartTurnOrder();
}

// Called every frame
void AGameManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::F))
    {
	
       // UE_LOG(LogTemp, Warning, "current TurnQueue size: %d", TurnQueue.Num());
		for (APlayerUnit* unit : TurnQueue)
		{
			UE_LOG(LogTemp, Warning, TEXT("Unit: %s"), *unit->GetName());
		}
		for (APlayerUnit* unit : PlayerUnits)
		{
			UE_LOG(LogTemp, Warning, TEXT("Player Unit: %s"), *unit->GetName());
		}
        UE_LOG(LogTemp, Warning, TEXT("Size of TurnQueue: %d"), TurnQueue.Num());
    }
}
