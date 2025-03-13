// Fill out your copyright notice in the Description page of Project Settings.

#include "CombatManager.h"
#include "Rat_Conquest/Player/PlayerCamera.h"
#include "Kismet/GameplayStatics.h"
#include "Rat_Conquest/Data/MutationData.h"
#include "Rat_Conquest/Unit/PlayerUnit.h"
#include "Rat_Conquest/Managers/GameManager/GameManager.h"
#include "Rat_Conquest/Items/Item.h"
#include "Rat_Conquest/Data/MutationData.h"

// Sets default values
ACombatManager::ACombatManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ACombatManager::DealDamageToUnit(APlayerUnit* Attackerunit, APlayerUnit* Defenderunit)
{
    if (!Attackerunit || !Defenderunit)
    {
        UE_LOG(LogTemp, Error, TEXT("DealDamageToUnit failed: Attacker or Defender is null!"));
        return;
    }

    int weaponDamage = 0;
    if (Attackerunit->ItemSlots[0])
    {
        AItem* item = Cast<AItem>(Attackerunit->ItemSlots[0]);
        if (item)
        {
            weaponDamage = item->Damage;
        }
    }
    int TotalDamage = Attackerunit->Damage + weaponDamage;

    if (TotalDamage >= Defenderunit->Health && Attackerunit->bIsPlayerUnit)
    {
        Attackerunit->mutationData->AddExperience(Defenderunit->experienceReward);
		Attackerunit->Mutate();
    }

    UE_LOG(LogTemp, Warning, TEXT("Dealing %d damage to %s"), TotalDamage, *Defenderunit->GetName());

    // Add a small knockback effect
    ApplyKnockback(Attackerunit, Defenderunit);

    // Apply damage to the defender
    HandleUnitDamage(Defenderunit, TotalDamage);
}

void ACombatManager::ApplyKnockback(APlayerUnit* Attackerunit, APlayerUnit* Defenderunit)
{
    if (!Attackerunit || !Defenderunit)
    {
        UE_LOG(LogTemp, Error, TEXT("ApplyKnockback failed: Attacker or Defender is null!"));
        return;
    }

    // Calculate knockback direction (from attacker to defender)
    FVector KnockbackDirection = (Defenderunit->GetActorLocation() - Attackerunit->GetActorLocation()).GetSafeNormal();
    KnockbackDirection.Z = 0; // Ensure knockback is only horizontal

    // Define knockback strength and duration
    float KnockbackDistance = 100.0f; // Adjust this value to control the knockback distance
    float KnockbackDuration = 0.2f;   // Adjust this value to control the knockback duration

    // Calculate the target knockback position
    FVector StartPosition = Defenderunit->GetActorLocation();
    FVector TargetPosition = StartPosition + (KnockbackDirection * KnockbackDistance);

    // Start the knockback interpolation
    Defenderunit->KnockbackStartPosition = StartPosition;
    Defenderunit->KnockbackTargetPosition = TargetPosition;
    Defenderunit->KnockbackProgress = 0.0f;
    Defenderunit->bIsKnockbackActive = true;

    // Set a timer to reset the position after the knockback effect
    FTimerHandle KnockbackResetTimer;
    FTimerDelegate ResetPositionDelegate;
    ResetPositionDelegate.BindUObject(this, &ACombatManager::ResetKnockbackPosition, Defenderunit);
    GetWorld()->GetTimerManager().SetTimer(KnockbackResetTimer, ResetPositionDelegate, KnockbackDuration, false);
}

void ACombatManager::ResetKnockbackPosition(APlayerUnit* Defenderunit)
{
    if (!Defenderunit)
    {
        UE_LOG(LogTemp, Error, TEXT("ResetKnockbackPosition failed: Defender is null!"));
        return;
    }

    // Reset the defender's position to its original tile
    Defenderunit->SetActorLocation(Defenderunit->KnockbackStartPosition);
    Defenderunit->bIsKnockbackActive = false;
}

void ACombatManager::HandleUnitDamage(APlayerUnit* unit, int amount)
{
	if (!unit)
	{
		UE_LOG(LogTemp, Error, TEXT("TakeDamage failed: Unit is null!"));
		return;
	}

	int TotalDamage = FMath::Max(amount - unit->Defence, 1);
	unit->Health -= TotalDamage;

    unit->OnHealthChanged.Broadcast();
    unit->UpdateHealthBar();
	UE_LOG(LogTemp, Warning, TEXT("took %d damage"),amount);

	if (unit->Health <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("has been killed!"));
		KillUnit(unit);
		
	}
	else
	{
		//UE_LOG(LogTemp, Display, TEXT(" survived with %d health remaining"), unit->health);
	}
}

void ACombatManager::KillUnit(APlayerUnit* unit)
{
    if (!unit)
    {
        UE_LOG(LogTemp, Error, TEXT("KillUnit failed: Unit is null!"));
        return;
    }

    // Clear player's current unit reference
    APlayerCamera* PlayerCharacter = Cast<APlayerCamera>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    if (PlayerCharacter)
    {
        PlayerCharacter->SetCurrentUnit(nullptr);
    }

    // Get the game manager
    AGameManager* GameManager = Cast<AGameManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameManager::StaticClass()));
    if (GameManager)
    {
        // Check if the killed unit was the current unit
        bool bWasCurrentUnit = (GameManager->CurrentUnit == unit);

        // Remove unit from queue
        GameManager->RemoveUnitFromQueue(unit);

        // If the killed unit was the current turn unit, properly move to the next turn
        if (bWasCurrentUnit)
        {
            GameManager->EndUnitTurn();
        }

        // Optionally update the turn order after a unit dies
        GameManager->UpdateTurnQueue();
    }

    // Destroy the unit (after playing death animation, if needed)
    unit->KillAfterAnim();
}



// Called when the game starts or when spawned
void ACombatManager::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Display, TEXT("CombatManager initialized and ready!"));
}

// Called every frame
void ACombatManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
