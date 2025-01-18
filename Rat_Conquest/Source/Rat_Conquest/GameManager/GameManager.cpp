// Fill out your copyright notice in the Description page of Project Settings.


#include "GameManager.h"
#include "Rat_Conquest/Unit/PlayerUnit.h"
#include "EngineUtils.h" 
// Sets default values
AGameManager::AGameManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
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
         }
         else
         {
             EnemyUnits.Add(Unit);
         }
    }
}

void AGameManager::StartTurnOrder()
{
    TurnQueue.Empty();

    if (bisPlayersturn) {

        TurnQueue.Append(PlayerUnits);
        TurnQueue.Append(EnemyUnits);

    }


    if (TurnQueue.Num() > 0) {
        CurrentUnit = TurnQueue[0];

    }
    ExecuteTurn();
    
}

void AGameManager::ExecuteTurn()
{
    if (!CurrentUnit)
        return;

    if (CurrentUnit->bIsPlayerUnit) {
        CurrentUnit->ExecutePlayerTurn();

    }
    else {
        CurrentUnit->ExecuteAITurn();

    }
   

   
}

void AGameManager::EndUnitTurn()
{

    // each unti should also have a finish turn that runs this function
    if (TurnQueue.Num() == 0) {
        return;
    }

    TurnQueue.RemoveAt(0);

    if (TurnQueue.Num() > 0) {
        CurrentUnit = TurnQueue[0];
        ExecuteTurn();

    }
    else {
        bisPlayersturn = !bisPlayersturn;
        StartTurnOrder();
    }
}

// Called when the game starts or when spawned
void AGameManager::BeginPlay()
{
    Super::BeginPlay();

    TogglePlayerTurn();
    InitalizeUnits();
    StartTurnOrder();

   
}

// Called every frame
void AGameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

