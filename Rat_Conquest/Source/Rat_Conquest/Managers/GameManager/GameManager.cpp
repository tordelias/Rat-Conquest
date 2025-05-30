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
#include "Sound/SoundBase.h"
#include "Rat_Conquest/Unit/UnitMarker.h"
#include "Rat_Conquest/Unit/GeneralAIUnit.h"
#include "Rat_Conquest/LevelObjects/InteractableGridObject.h"
#include "Rat_Conquest/Unit/MyBossUnit.h"
#include "Components/AudioComponent.h"


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
    
	//UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 2.0f); // Reset time dilation to normal speed
    //InitalizeUnits();
    TArray<AActor*> FoundGridManagers;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGridManager::StaticClass(), FoundGridManagers);

    // create the UniutMarker
    unitMarker = GetWorld()->SpawnActor<AUnitMarker>(AUnitMarker::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);

    if (FoundGridManagers.Num() > 0)
    {
        GridManager = TObjectPtr<AGridManager>(Cast<AGridManager>(FoundGridManagers[0]));
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
 
    //Delay MarkCurrentUnit(); to next tick
    FTimerHandle DelayTimer;
    GetWorld()->GetTimerManager().SetTimer(
		DelayTimer,
		this,
		&AGameManager::MarkCurrentUnit,
		0.1f, // delay in seconds
		false // no looping
	);
}

void AGameManager::TogglePlayerTurn()
{
    bisPlayersturn = true;

}

void AGameManager::SetGameDifficulty(int difficulty)
{
    //Print difficulty to screen
	GameDifficulty = difficulty;
}

void AGameManager::InitalizeUnits()
{
  
    PlayerUnits.Empty();
    EnemyUnits.Empty();
	int UnitNum = 0;
    for (TActorIterator<APlayerUnit> It(GetWorld()); It; ++It)
    {
        APlayerUnit* Unit = *It;
        if (Unit && Unit->bIsPlayerUnit)
        {
            
            PlayerUnits.Add(Unit);
            Unit->Seticon(UnitNum); 
			++UnitNum;
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

    if(!LevelGenerator)
        RotateUnits(180);

}

void AGameManager::StartTurnOrder()
{
    MainHUD = Cast<AMainHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
    TurnQueue.Empty();

    // Create a list of all units
    TArray<TObjectPtr<APlayerUnit>> AllUnits;
    AllUnits.Append(PlayerUnits);
    AllUnits.Append(EnemyUnits);

    // Filter out null units
    AllUnits.RemoveAll([](TObjectPtr<APlayerUnit> Unit) { return !IsValid(Unit); });

    // Sort by Initiative, with PlayerUnits going first in case of ties
    AllUnits.Sort([](const APlayerUnit& A, const APlayerUnit& B) {
        if (A.Initiative + A.InitiativeFromItems == B.Initiative + B.InitiativeFromItems) {
            return A.bIsPlayerUnit && !B.bIsPlayerUnit; // Player goes first in tie
        }
        return A.Initiative + A.InitiativeFromItems > B.Initiative + B.InitiativeFromItems; // Higher Initiative goes first
        });

    // Fill turn queue ensuring all units go before any repeat turns
    for (int32 i = 0; i < VisibleTurnsAhead / AllUnits.Num(); i++)
    {
        TurnQueue.Append(AllUnits);
    }

    // Trim to visible limit
    TurnQueue.SetNum(FMath::Min(VisibleTurnsAhead, TurnQueue.Num()));

    // Update UI
    if (MainHUD)
    {
        for (TObjectPtr<APlayerUnit> Unit : TurnQueue)
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

    // Reset turn state for the unit
    CurrentUnit->bHasFinishedTurn = false;
	//MarkCurrentUnit();
    APlayerCamera* PlayerCharacter = Cast<APlayerCamera>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    if (CurrentUnit->bIsPlayerUnit)
    {
        if (PlayerCharacter)
        {
            PlayerCharacter->SetCurrentUnit(CurrentUnit); // Allow player control
            CurrentUnit->bIsCurrentUnit = true;
        }
        CurrentUnit->ExecutePlayerTurn();
        UE_LOG(LogTemp, Warning, TEXT("Player unit turn executed"));
        if (SB_PlayerTurn)
        {
            float RandomPitch = FMath::FRandRange(0.95f, 1.1f); // Slight random pitch shift
            UAudioComponent* AudioComp = UGameplayStatics::SpawnSound2D(GetWorld(), SB_PlayerTurn);

            if (AudioComp)
            {
                AudioComp->SetPitchMultiplier(RandomPitch);
            }
        }
    }
    else
    {
        if (PlayerCharacter && IsValid(PlayerCharacter->GetCurrentUnit().Get()) && !PlayerCharacter->GetCurrentUnit()->bIsPlayerUnit)
        {
            PlayerCharacter->SetCurrentUnit(nullptr);
        }

        float RandomDelay = FMath::RandRange(0.90f, 1.4f);
        FTimerHandle AIUnitTurnTimerHandle;
        GetWorld()->GetTimerManager().SetTimer(
            AIUnitTurnTimerHandle,
            this,
            &AGameManager::HandleAITurnAfterDelay,
            RandomDelay,
            false
        );
		if (SB_EnemyTurn)
		{

            float RandomPitch = FMath::FRandRange(0.95f, 1.05f); // Slight random pitch shift
            UAudioComponent* AudioComp = UGameplayStatics::SpawnSound2D(GetWorld(), SB_EnemyTurn);

            if (AudioComp)
            {
                AudioComp->SetPitchMultiplier(RandomPitch);
            }
		}
    }

    if (EnemyUnits.Num() == 0)
    {
        bEncounterComplete = true;
        if (bTestEncounter && MainHUD)
        {
            MainHUD->ShowVictoryWidget();
        }
        if (LevelGenerator)
        {
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
	APlayerCamera* PlayerCharacter = Cast<APlayerCamera>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (PlayerCharacter)
	{
		PlayerCharacter->SetCurrentUnit(nullptr);
	}
	for (TObjectPtr<APlayerUnit> enemy : EnemyUnits)
	{
		if (enemy)
		{
			enemy->bHasFinishedTurn = false;
		}
	}

    if (EnemyUnits.Num() == 0) {
        bEncounterComplete = true;
        if (PlayerCharacter) {
            PlayerCharacter->SetCameraTopDown(-90, 1000);

        }
        if (bTestEncounter) {
            if (MainHUD) {
                MainHUD->ShowVictoryWidget();
            }
        }
        if (LevelGenerator && RoomsExplored < MaxRoomsToExplore && !bTestEncounter) {
            LevelGenerator->SetupRoomSelectUI();
        }
        else if (RoomsExplored > MaxRoomsToExplore && !bTestEncounter) {
            //Show victory screen
            if (MainHUD) {
                MainHUD->ShowVictoryWidget();
            }
        }
    }

    if (TurnQueue.Num() > 0)
    {
        TurnQueue.RemoveAt(0);

        if (MainHUD)
        {
            MainHUD->RemoveTurnImage();
        }

        if (TurnQueue.Num() > 0) // Ensure the queue is not empty before accessing index 0
        {
            CurrentUnit = TurnQueue[0];
            if (IsValid(CurrentUnit))
            {
                HighlightUnitAndTiles(CurrentUnit);
                MarkCurrentUnit();
				//Add a 1 secound delay before executing the next tur*�
				FTimerHandle NextTurnTimerHandle;
				GetWorld()->GetTimerManager().SetTimer(NextTurnTimerHandle, this, &AGameManager::ExecuteTurn,1.7f, false);
                //ExecuteTurn();
            }
        }
        else
        {
            bisPlayersturn = !bisPlayersturn;
            StartTurnOrder();  // Restart the turn order if queue is empty
        }
    }
    else
    {
        bisPlayersturn = !bisPlayersturn;
        StartTurnOrder();
    }

}



void AGameManager::RemoveUnitFromQueue(TObjectPtr<APlayerUnit> Unit)
{
    if (!Unit) return;

    // Remove all instances of the unit from the turn queue
    TurnQueue.RemoveAll([Unit](TObjectPtr<APlayerUnit> Entry) {
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
    //UpdateTurnQueue();
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
    for (TObjectPtr<APlayerUnit> Unit : EnemyUnits)
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
    
    if (GridManager && GridManager->IsGridFinished() && bLevelFinishedGenerating)
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
	GridManager->ClearUnitsFromTile();
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
	UE_LOG(LogTemp, Warning, TEXT("Loading explored encounter"));
}



void AGameManager::StartEncounter()
{
    if (MainHUD) MainHUD->ClearTurnImages();
    if (!GridManager) return;
    bisPlayersturn = true;
    GridManager->ScanWorldForObjects();
    //Spawn new enemies
    if (RoomsExplored == MaxRoomsToExplore + 1) {
		//Spawn boss Enemy instead of normal enemies

        if (BossList.Num() > 0) {
            int RandomIndex = FMath::RandRange(0, BossList.Num() - 1);

            FVector SpawnLocation = FVector(0.f, 50.f, 0.f); // Adjust as needed
            FRotator SpawnRotation = FRotator::ZeroRotator;

            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = this;
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

            TObjectPtr<APlayerUnit> NewEnemy = GetWorld()->SpawnActor<APlayerUnit>(
                BossList[RandomIndex],
                SpawnLocation,
                SpawnRotation,
                SpawnParams
            );
            if (NewEnemy->GridManager.IsValid() && NewEnemy->GridManager->bIsGridScanned)
            {
                Cast<AMyBossUnit>(NewEnemy)->AIDifficulty = GameDifficulty;
                NewEnemy->SpawnDefaultController();

            }
            else {
                UE_LOG(LogTemp, Error, TEXT("Failed to get GM for enemy"));
            }
            if (AEnemyAIController* AIController = Cast<AEnemyAIController>(NewEnemy->GetController()))
            {
                AIController->Possess(NewEnemy);
            }

        }



    }
    else {
        for (int i = 0; i < FMath::RandRange(1, 2 + GameDifficulty); ++i)
        {
            if (EnemyList.Num() > 0)
            {
                int RandomIndex = FMath::RandRange(0, EnemyList.Num() - 1);
                if (RoomsExplored == 0) 
                {
                    RandomIndex = 0;
                
                }


                FVector SpawnLocation = FVector(0.f, 50.f, 0.f); // Adjust as needed
                FRotator SpawnRotation = FRotator::ZeroRotator;

                FActorSpawnParameters SpawnParams;
                SpawnParams.Owner = this;
                SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

                TObjectPtr<APlayerUnit> NewEnemy = GetWorld()->SpawnActor<APlayerUnit>(
                    EnemyList[RandomIndex],
                    SpawnLocation,
                    SpawnRotation,
                    SpawnParams
                );
                if (NewEnemy->GridManager.IsValid() && NewEnemy->GridManager->bIsGridScanned)
                {
                    Cast<AGeneralAIUnit>(NewEnemy)->AIDifficulty = GameDifficulty;
                    NewEnemy->SpawnDefaultController();

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
    }
	

  
   
   

    for (APlayerUnit* P_unit : PlayerUnits)
    {
        P_unit->GridStartPosition = FVector2D(0, 0);

        // Incremental delay for each unit
        FTimerHandle AIUnitTurnTimerHandle;
        if (P_unit->GridManager.IsValid() && GridManager->bIsGridScanned) {
           
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

    for (int i = 0; i < FMath::RandRange(1, 2); ++i) {

        FTimerHandle LootSpawnTimer;
        GetWorld()->GetTimerManager().SetTimer(
            LootSpawnTimer,
            this,
            &AGameManager::DelayedSpawnLoot,
            0.50f, // delay in seconds
            false // no looping
        );
    }
	for (int i = 0; i < FMath::RandRange(1, 2); ++i) {
		FTimerHandle InteractableSpawnTimer;
		GetWorld()->GetTimerManager().SetTimer(
			InteractableSpawnTimer,
			this,
			&AGameManager::DelayedSpawnInteractableObjects,
			0.50f, // delay in seconds
			false // no looping
		);
	}
    RotateUnits(0);
   
}

void AGameManager::UpdateTurnQueue()
{
    // Clear UI first
    if (MainHUD) {
        MainHUD->ClearTurnImages(); // Make sure this is clearing all previous turn indicators
    }

    // Create a fresh list of valid units for the current turn order
    TArray<TObjectPtr<APlayerUnit>> ValidUnits;
    if (bisPlayersturn) {
        ValidUnits.Append(PlayerUnits);
        ValidUnits.Append(EnemyUnits);
    }
    else {
        ValidUnits.Append(PlayerUnits);
        ValidUnits.Append(EnemyUnits);
       
    }

    // Remove invalid units (this is important to ensure we don�t display dead units)
    ValidUnits.RemoveAll([](TObjectPtr<APlayerUnit> Unit) { return !IsValid(Unit); });

    // Clear previous queue and regenerate it
    TurnQueue.Empty();
    for (int32 i = 0; i < VisibleTurnsAhead; i++) {
        TurnQueue.Append(ValidUnits); // Append the units repeatedly to simulate the upcoming turns
    }
    TurnQueue.SetNum(FMath::Min(VisibleTurnsAhead, TurnQueue.Num()));

    // Now add the new valid turn images to the HUD
    if (MainHUD) {
        for (TObjectPtr<APlayerUnit> Unit : TurnQueue) {
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
    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (PlayerController && CurrentUnit)
    {
       
        
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
    SpawnLocation.Z += 45;
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

void AGameManager::SpawnInteractableObjects()
{
    if (!GridManager || InteractObjPool.Num() == 0)
        return; // Ensure GridManager and Pool exist

	FVector SpawnLocation = GridManager->GetRandomPositionInGrid();
	if (SpawnLocation == FVector())
		return; // No valid unoccupied positions available
	//SpawnLocation.Z += 45;
	int RandomIndex = FMath::RandRange(0, InteractObjPool.Num() - 1);
    if (PlayerUnits.Num() > 3 && RandomIndex == 1 || RoomsExplored == 0) {
        RandomIndex = 0;
    }
	FRotator SpawnRotation = FRotator::ZeroRotator;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AInteractableGridObject* NewInteractable = GetWorld()->SpawnActor<AInteractableGridObject>(
		InteractObjPool[RandomIndex],
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	);
	if (NewInteractable)
	{
		NewInteractable->SetActorLocation(SpawnLocation);
		NewInteractable->SetActorRotation(SpawnRotation);
		GridManager->ScanWorldForObjects();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn interactable object"));
	}

}

void AGameManager::DelayedSpawnLoot()
{
    SpawnLoot();
}

void AGameManager::DelayedSpawnInteractableObjects()
{
	SpawnInteractableObjects();
}

void AGameManager::SpawnNewPlayerUnit(FVector2D Gridposition)
{
	if (!GridManager) return;
	TWeakObjectPtr<AActor> GridTile = GridManager->GetClosestAvailableTile(Gridposition);
	if (!GridTile.IsValid())
		return; // No valid unoccupied positions available
	// Check if the tile is valid and not occupied
	AGridTile* GridTileActor = Cast<AGridTile>(GridTile.Get());
	if (!GridTileActor || GridTileActor->bIsOccupied)
		return; // No valid unoccupied positions available
	// Check if the tile is valid and not occupied
	if (GridTileActor->bIsOccupied)
		return; // No valid unoccupied positions available
	FVector SpawnLocation = GridTileActor->GetActorLocation();
	FVector2D GridPosition = GridTileActor->GridPosition;
	if (SpawnLocation == FVector())
		return; // No valid unoccupied positions available
	//SpawnLocation.Z += 4;
	FRotator SpawnRotation = FRotator::ZeroRotator;
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	APlayerUnit* NewPlayerUnit = GetWorld()->SpawnActor<APlayerUnit>(
        NewPlayerUnitList[0],
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	);
	if (NewPlayerUnit)
	{
		
		//NewPlayerUnit->SetActorLocation(SpawnLocation);
		NewPlayerUnit->SetActorRotation(SpawnRotation);
		//NewPlayerUnit->CurrentGridPosition = GridPosition;
		PlayerUnits.Add(NewPlayerUnit);
		NewPlayerUnit->Seticon(PlayerUnits.Num() - 1);
		UE_LOG(LogTemp, Warning, TEXT("New player unit spawned: %s"), *NewPlayerUnit->GetName());
		int UnitNum = 0;
		GridTileActor->bIsOccupied = true;
       /* for (TActorIterator<APlayerUnit> It(GetWorld()); It; ++It)
        {
            APlayerUnit* Unit = *It;
            if (Unit && Unit->bIsPlayerUnit)
            {

                
                Unit->Seticon(UnitNum);
                ++UnitNum;
                UE_LOG(LogTemp, Warning, TEXT("Player unit added"));
            }
        }*/
        for (APlayerUnit* Unit : PlayerUnits)
        {
            if (MainHUD)
            {
                MainHUD->AddPlayerUnitToScreen(Unit);
            }
        }
        
	}
}

void AGameManager::RotateUnits(float roation)
{

	for (TObjectPtr<APlayerUnit> unit : PlayerUnits)
	{
		if (unit)
		{
            UE_LOG(LogTemp, Error, TEXT("Rotating units degrees"));
			unit->SetTurnAngle(roation);
			unit->SetActorRotation(FRotator(0, roation, 0));
		}
	}
    FTimerHandle RotateDelayTimer;
    GetWorld()->GetTimerManager().SetTimer(
        RotateDelayTimer,
        this,
        &AGameManager::RotateEnemyUnits,
        0.40f, // delay in seconds
        false // no looping
    );
}

void AGameManager::RotateEnemyUnits()
{
    float rotation = 0;

    if (PlayerUnits.Num() > 0 && PlayerUnits[0]) // check if array has an element first
    {
        rotation = PlayerUnits[0]->GetActorRotation().Yaw;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("RotateEnemyUnits called but no PlayerUnits available!"));
        return;
    }

    for (TObjectPtr<APlayerUnit> unit : EnemyUnits)
    {
        if (unit && !unit->bIsPlayerUnit)
        {
            unit->SetTurnAngle(rotation);
            unit->SetActorRotation(FRotator(0, rotation + 180, 0));
        }
    }
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

void AGameManager::MarkCurrentUnit()
{
    if (IsValid(unitMarker.Get()) && IsValid(CurrentUnit.Get()))
    {
        unitMarker->SetPosition(CurrentUnit->GetActorLocation());
        unitMarker->SetTargetActor(CurrentUnit);

        // Increase widget size for currentUnit
        if (IsValid(MainHUD.Get()))
        {
            MainHUD->HighlightPlayerUnit(CurrentUnit);
        }

        //if (CurrentUnit->bIsPlayerUnit)
        //{
            // Cast to playerCamera
            APlayerCamera* PlayerCharacter = Cast<APlayerCamera>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
            if (PlayerCharacter)
            {
                GetWorld()->GetTimerManager().SetTimer(
                    CameraDelayTimerHandle,
                    [this, PlayerCharacter]()
                    {
                        if (IsValid(PlayerCharacter) && IsValid(CurrentUnit.Get()))
                        {
                            PlayerCharacter->SetCameraPosition(CurrentUnit->GetActorLocation());
                        }
                    },
					1.0f, // Delay before moving the camera
                    false
                );
            }
        //}
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Unit marker is not valid"));
    }
}


// Called every frame
void AGameManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::F))
    {
	  
       // UE_LOG(LogTemp, Warning, "current TurnQueue size: %d", TurnQueue.Num());
		for (TObjectPtr<APlayerUnit> unit : TurnQueue)
		{
			UE_LOG(LogTemp, Warning, TEXT("Unit: %s"), *unit->GetName());
		}
		for (TObjectPtr<APlayerUnit> unit : PlayerUnits)
		{
			UE_LOG(LogTemp, Warning, TEXT("Player Unit: %s"), *unit->GetName());
            
		}
        UE_LOG(LogTemp, Warning, TEXT("Size of TurnQueue: %d"), TurnQueue.Num());
        UE_LOG(LogTemp, Display, TEXT("Current Turn Queue:"));
        //SpawnLoot();
        APlayerCamera* PlayerCharacter = Cast<APlayerCamera>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
        if (PlayerCharacter) {
            PlayerCharacter->SetCameraTopDown(-90, 1000);

        }
        RotateEnemyUnits();
      
       
    }
	isUnitAlive();
}
