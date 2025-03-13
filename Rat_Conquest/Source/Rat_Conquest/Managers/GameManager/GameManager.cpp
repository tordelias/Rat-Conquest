// Fill out your copyright notice in the Description page of Project Settings.

#include "GameManager.h"
#include "Rat_Conquest/Unit/PlayerUnit.h"
#include "Rat_Conquest/Player/PlayerCamera.h"
#include "Kismet/GameplayStatics.h"
#include "Rat_Conquest/Managers/GridManager/GridManager.h"
#include "Rat_Conquest/Map/LevelGenerator.h"
#include "Rat_Conquest/GridTile/GridTile.h"
#include "EngineUtils.h"
#include "Rat_Conquest/Widgets/MainHUD.h"
#include "Rat_Conquest/Items/Item.h"
#include "Rat_Conquest/Widgets/MainWidget.h"
#include "Rat_Conquest/Widgets/TurnIndicatorWidget.h"
#include "Rat_Conquest/AI/EnemyAIController.h"
// Sets default values
AGameManager::AGameManager()
{
    // Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

}
// Called when the game starts or when spawned
void AGameManager::BeginPlay()
{
    Super::BeginPlay();
    
	
    //InitalizeUnits();
    TArray<AActor*> FoundGridManagers;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGridManager::StaticClass(), FoundGridManagers);

    if (FoundGridManagers.Num() > 0)
    {
        GridManager = Cast<AGridManager>(FoundGridManagers[0]);
        if (FoundGridManagers.Num() > 1)
        {
            UE_LOG(LogTemp, Warning, TEXT("Multiple GridManagers found! Using first instance."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("No GridManager found in the level!"));
    }

    TArray<AActor*> FoundLevelGen;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALevelGenerator::StaticClass(), FoundLevelGen);

    if (FoundLevelGen.Num() > 0)
    {
        LevelGenerator = Cast<ALevelGenerator>(FoundLevelGen[0]);
        if (FoundLevelGen.Num() > 1)
        {
            UE_LOG(LogTemp, Warning, TEXT("Multiple GridManagers found! Using first instance."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("No GridManager found in the level!"));
    }

    if (bTestEncounter) {
        GridManager->ScanWorldForObjects();
        GetWorldTimerManager().SetTimerForNextTick(this, &AGameManager::InitalizeUnits);
    }
    else {
        CheckForEncounter();
    }
   
    
 

  
    
	
}

void AGameManager::TogglePlayerTurn()
{
    bisPlayersturn = true;

}

void AGameManager::InitalizeUnits()
{
  
    PlayerUnits.Empty();
    EnemyUnits.Empty();
    for (TActorIterator<APlayerUnit> It(GetWorld()); It; ++It)
    {
        APlayerUnit* Unit = *It;
        if (Unit && Unit->bIsPlayerUnit)
        {
            
            PlayerUnits.Add(Unit);
            UE_LOG(LogTemp, Warning, TEXT("Player unit added"));
        }
        else if (Unit)
        {
            EnemyUnits.Add(Unit);
            UE_LOG(LogTemp, Warning, TEXT("Enemy unit added"));
        }

    }
    if (bTestEncounter) {
        GridManager->ScanWorldForObjects();
    }
    TogglePlayerTurn();
    StartTurnOrder();
	for (APlayerUnit* Unit : PlayerUnits)
	{
        if (MainHUD)
        {
            MainHUD->AddPlayerUnitToScreen(Unit);
        }
	}

}

void AGameManager::StartTurnOrder()
{
    MainHUD = Cast<AMainHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
    TurnQueue.Empty();

    // Create fresh turn order
    TArray<APlayerUnit*> AllUnits;
    if (bisPlayersturn)
    {
        AllUnits.Append(PlayerUnits);
        AllUnits.Append(EnemyUnits);
    }
    else
    {
        AllUnits.Append(PlayerUnits);
        AllUnits.Append(EnemyUnits);
       
    }

    // Filter out null units
    AllUnits.RemoveAll([](APlayerUnit* Unit) { return !IsValid(Unit); });

    // Create turn queue for multiple rounds
    for (int32 i = 0; i < VisibleTurnsAhead; i++)
    {
        for (APlayerUnit* Unit : AllUnits)
        {
            if (IsValid(Unit))
            {
                TurnQueue.Add(Unit);
            }
        }
    }
    
    // Trim to visible limit
    if (TurnQueue.Num() > VisibleTurnsAhead)
    {
        TurnQueue.SetNum(VisibleTurnsAhead);
    }

    // Update UI
    if (MainHUD)
    {
       
        for (APlayerUnit* Unit : TurnQueue)
        {
            if (IsValid(Unit))
            {
                MainHUD->AddTurnImage(Unit);
            }
        }
    }

    if (TurnQueue.Num() > 0)
    {
        CurrentUnit = TurnQueue[0];
        HighlightUnitAndTiles(CurrentUnit);
        ExecuteTurn();
    }
}

void AGameManager::ExecuteTurn()
{
    if (bIsGamePaused)
    {
        UE_LOG(LogTemp, Warning, TEXT("Game is paused. Turn execution halted."));
        return;
    }

    if (!CurrentUnit) return;

    if (CurrentUnit->bIsPlayerUnit)
    {
        APlayerCamera* PlayerCharacter = Cast<APlayerCamera>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
        if (PlayerCharacter)
        {
            PlayerCharacter->SetCurrentUnit(CurrentUnit);
            CurrentUnit->bIsCurrentUnit = true;
        }
        CurrentUnit->ExecutePlayerTurn();
        UE_LOG(LogTemp, Warning, TEXT("Player unit turn executed"));
    }
    else
    {
        //Adds a random delay to the AI unit's turn 
        float RandomDelay = FMath::RandRange(0.90f, 1.9f);
        FTimerHandle AIUnitTurnTimerHandle;
        GetWorld()->GetTimerManager().SetTimer(
            AIUnitTurnTimerHandle,
            this,
            &AGameManager::HandleAITurnAfterDelay,
            RandomDelay,
            false
        );
    }
    if (EnemyUnits.Num() == 0) {
        bEncounterComplete = true;
        if (LevelGenerator) {
            LevelGenerator->SetupRoomSelectUI();
        }
    }
}


void AGameManager::EndUnitTurn()
{
    if (!CurrentUnit) return;

    // Clear current unit state
    CurrentUnit->bIsCurrentUnit = false;
    CurrentUnit = nullptr;

    if (EnemyUnits.Num() == 0) {
        bEncounterComplete = true;
        APlayerCamera* PlayerCharacter = Cast<APlayerCamera>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
        if (PlayerCharacter) {
            PlayerCharacter->SetCameraTopDown(0, 100);

        }
        if (LevelGenerator && RoomsExplored < 3) {
            LevelGenerator->SetupRoomSelectUI();
        }
        else if (RoomsExplored <= 3) {
            //Show victory screen
            if (MainHUD) {
                MainHUD->ShowVictoryWidget();
            }
        }
    }

    if (TurnQueue.Num() > 0)
    {
        // Remove the front unit (current) and get next
        TurnQueue.RemoveAt(0);

        if (MainHUD)
        {
            MainHUD->RemoveTurnImage();  // Remove the image for the unit that just finished its turn
        }

        if (TurnQueue.Num() > 0)
        {
            CurrentUnit = TurnQueue[0];
            HighlightUnitAndTiles(CurrentUnit);
            ExecuteTurn();
        }
        else
        {
            bisPlayersturn = !bisPlayersturn;
            StartTurnOrder();  // Restart the turn order if queue is empty
        }

        // Update turn queue UI after removing a unit
        //MainHUD->RemoveTurnImage(); 
    }
    else
    {
        bisPlayersturn = !bisPlayersturn;
        StartTurnOrder();  // Restart the turn order if queue is empty
    }
}



void AGameManager::RemoveUnitFromQueue(APlayerUnit* Unit)
{
    if (!Unit) return;

    // Remove all instances of the unit from the turn queue
    TurnQueue.RemoveAll([Unit](APlayerUnit* Entry) {
        return Entry == Unit;
        });

    // Remove from team lists
    if (Unit->bIsPlayerUnit) {
        PlayerUnits.Remove(Unit);
    }
    else {
        EnemyUnits.Remove(Unit);
    }

    // Handle current unit
    if (CurrentUnit == Unit) {
        CurrentUnit->FinishTurn();
        EndUnitTurn(); // Ensures turn progression
    }

    // Now, instead of manually handling UI updates here, delegate it to the widget's method
    if (MainHUD) {
        MainHUD->RemoveUnitFromQueue(Unit);  // Call the widget method to handle UI updates
    }

    // Update the turn queue after removing a unit
    UpdateTurnQueue();
}




void AGameManager::HandleAITurnAfterDelay()
{
    if (CurrentUnit)
    {
        CurrentUnit->ExecuteAITurn();  // Perform the AI unit's actions
        UE_LOG(LogTemp, Warning, TEXT("AI unit turn executed"));

        // Proceed to the next turn after the AI completes its turn
		
    }
}

void AGameManager::EndEncounter()
{
	// Clear the turn queue
	TurnQueue.Empty();

	// Clear the player and enemy units
    for (APlayerUnit* Unit : EnemyUnits)
    {
        if (Unit) Unit->Destroy();
    }
	PlayerUnits.Empty();
	EnemyUnits.Empty();

	// Clear the currently focused unit and tiles
	CurrentlyFocusedUnit = nullptr;
	CurrentlyFocusedTiles.Empty();


	
	
}

void AGameManager::CheckForEncounter()
{
    
    if (GridManager && GridManager->bIsGridFinished() && bLevelFinishedGenerating)
    {
        
        StartEncounter();
    }
    else
    {

       
       GetWorldTimerManager().SetTimerForNextTick(this, &AGameManager::CheckForEncounter);
       
    }
}

void AGameManager::LoadNextEncounter()
{
    bEncounterComplete = false;
    GridManager->bIsGridScanned = false;
    GridManager->ScanWorldForObjects();
    CheckForEncounter();
}

void AGameManager::LoadExploredEncounter()
{
    GridManager->bIsGridScanned = false;
    GridManager->ScanWorldForObjects();
    bEncounterComplete = true;
    if (LevelGenerator) {
        LevelGenerator->SetupRoomSelectUI();
    }
}



void AGameManager::StartEncounter()
{
    if (MainHUD) MainHUD->ClearTurnImages();
    if (!GridManager) return;
    bisPlayersturn = true;
    GridManager->ScanWorldForObjects();
    //Spawn new enemies
	for (int i = 0; i < 1; ++i)
	{
		if (EnemyList.Num() > 0)
		{
            int RandomIndex = FMath::RandRange(0, EnemyList.Num() - 1);

            FVector SpawnLocation = FVector(0.f, 50.f, 0.f); // Adjust as needed
            FRotator SpawnRotation = FRotator::ZeroRotator;

            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = this;
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

            APlayerUnit* NewEnemy = GetWorld()->SpawnActor<APlayerUnit>(
                EnemyList[RandomIndex],
                SpawnLocation,
                SpawnRotation,
                SpawnParams
            );
            if (NewEnemy->GridManager && NewEnemy->GridManager->bIsGridScanned) {

                NewEnemy->SpawnDefaultController();
                NewEnemy->DelayedInitalPosition();
                UE_LOG(LogTemp, Error, TEXT("SET POSITON enemy"));
            }
            else {
                UE_LOG(LogTemp, Error, TEXT("Failed to get GM for enemy"));
            }
         

            
            if (AEnemyAIController* AIController = Cast<AEnemyAIController>(NewEnemy->GetController()))
            {
                AIController->Possess(NewEnemy);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to get AI Controller for spawned enemy"));
            }
			if (NewEnemy)
			{
				UE_LOG(LogTemp, Warning, TEXT("New enemy spawned: %s"), *NewEnemy->GetName());
				
			}
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to spawn enemy at index %d"), RandomIndex);
            }
            
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No enemies in the EnemyList!"));
		}
	}

  
   
   

    for (APlayerUnit* P_unit : PlayerUnits)
    {
        P_unit->GridStartPosition = FVector2D(0, 0);

        // Incremental delay for each unit
        FTimerHandle AIUnitTurnTimerHandle;
        if (P_unit->GridManager && GridManager->bIsGridScanned) {
           
            P_unit->DelayedInitalPosition();
            // Increase the delay for the next unit
            UE_LOG(LogTemp, Warning, TEXT("MOVE PLAYER!"))
        }
        else {
            UE_LOG(LogTemp, Warning, TEXT("COULD NOT MOVE PLAYER!"));
        }
       

       
    }
    GetWorldTimerManager().SetTimerForNextTick(this, &AGameManager::InitalizeUnits);
    //// Initialize the player and enemy units
   

}

void AGameManager::UpdateTurnQueue()
{
    // Clear UI first
    if (MainHUD) {
        MainHUD->ClearTurnImages(); // Make sure this is clearing all previous turn indicators
    }

    // Create a fresh list of valid units for the current turn order
    TArray<APlayerUnit*> ValidUnits;
    if (bisPlayersturn) {
        ValidUnits.Append(PlayerUnits);
        ValidUnits.Append(EnemyUnits);
    }
    else {
        ValidUnits.Append(PlayerUnits);
        ValidUnits.Append(EnemyUnits);
       
    }

    // Remove invalid units (this is important to ensure we don’t display dead units)
    ValidUnits.RemoveAll([](APlayerUnit* Unit) { return !IsValid(Unit); });

    // Clear previous queue and regenerate it
    TurnQueue.Empty();
    for (int32 i = 0; i < VisibleTurnsAhead; i++) {
        TurnQueue.Append(ValidUnits); // Append the units repeatedly to simulate the upcoming turns
    }
    TurnQueue.SetNum(FMath::Min(VisibleTurnsAhead, TurnQueue.Num()));

    // Now add the new valid turn images to the HUD
    if (MainHUD) {
        for (APlayerUnit* Unit : TurnQueue) {
            if (IsValid(Unit)) {
                MainHUD->AddTurnImage(Unit); // Add turn indicator for each valid unit
            }
        }
    }

    // Set the new current unit
    if (TurnQueue.Num() > 0) {
        CurrentUnit = TurnQueue[0];
        HighlightUnitAndTiles(CurrentUnit);
    }
}


void AGameManager::GenerateTurnBuffer()
{
    while (MasterTurnQueue.Num() < TurnBufferSize) {
       
    }
}

void AGameManager::PauseGame()
{
    bIsGamePaused = true;

    // Disable player input
    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    //if (PlayerController)
    //{
    //    PlayerController->SetPause(true);
    //    PlayerController->bShowMouseCursor = true; // Show cursor for stat selection
    //}

    UE_LOG(LogTemp, Warning, TEXT("Game Paused"));
}


void AGameManager::ResumeGame()
{
    bIsGamePaused = false;

    // Enable player input again
    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    //if (PlayerController)
    //{
    //    PlayerController->SetPause(false);
    //    PlayerController->bShowMouseCursor = false; // Hide cursor after selection
    //}

    UE_LOG(LogTemp, Warning, TEXT("Game Resumed"));

    // Continue turn execution
    if (CurrentUnit)
    {
        ExecuteTurn();
    }
}

bool AGameManager::isUnitAlive()
{
    if (!MainHUD)
        return false;

	if (PlayerUnits.Num() > 0)
	{
		return true;
	}
	MainHUD->ShowGameOverWidget();
    return false;
}

void AGameManager::SpawnLoot()
{
    if (!GridManager || ItemPool.Num() == 0)
        return; // Ensure GridManager and ItemPool exist

    FVector SpawnLocation = GridManager->GetRandomPositionInGrid();
    if (SpawnLocation == FVector())
        return; // No valid unoccupied positions available

    int RandomIndex = FMath::RandRange(0, ItemPool.Num() - 1);

    FRotator SpawnRotation = FRotator::ZeroRotator;

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;


    AItem* NewItem = GetWorld()->SpawnActor<AItem>(
        ItemPool[RandomIndex],
        SpawnLocation,
        SpawnRotation,
        SpawnParams
    );
   
    GridManager->ScanWorldForObjects();
}


void AGameManager::HighlightUnitAndTiles(APlayerUnit* NewUnit)
{
    // Unhighlight the previous unit and its tiles
    if (CurrentlyFocusedUnit)
    {
        CurrentlyFocusedUnit->EndFocus();
		UE_LOG(LogTemp, Warning, TEXT("EndFocus called")); 
        CurrentlyFocusedTiles.Empty();
    }

    // Highlight the new unit
    if (NewUnit)
    {
        CurrentlyFocusedUnit = NewUnit;
        CurrentlyFocusedUnit->BeginFocus();

    }
    else
    {
        CurrentlyFocusedUnit = nullptr;
    }
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
        UE_LOG(LogTemp, Display, TEXT("Current Turn Queue:"));
        SpawnLoot();
       
       
    }
	isUnitAlive();
}
