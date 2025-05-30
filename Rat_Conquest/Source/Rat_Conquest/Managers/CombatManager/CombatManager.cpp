// Fill out your copyright notice in the Description page of Project Settings.

#include "CombatManager.h"
#include "Rat_Conquest/Player/PlayerCamera.h"
#include "Kismet/GameplayStatics.h"
#include "Rat_Conquest/Data/MutationData.h"
#include "Rat_Conquest/Unit/PlayerUnit.h"
#include "Rat_Conquest/Managers/GameManager/GameManager.h"
#include "Rat_Conquest/Items/Item.h"
#include "Rat_Conquest/Data/MutationData.h"
#include "Sound/SoundBase.h"
// Sets default values
ACombatManager::ACombatManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ACombatManager::DealDamageToUnit(TWeakObjectPtr<APlayerUnit> Attackerunit, TWeakObjectPtr<APlayerUnit> Defenderunit)
{
    if (!Attackerunit.IsValid() || !Defenderunit.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("DealDamageToUnit failed: Attacker or Defender is null!"));
        return;
    }

    // Calculate weapon damage with random variance
    int weaponDamage = 0;
    if (Attackerunit->ItemSlots[0].IsValid())
    {
        AItem* item = Cast<AItem>(Attackerunit->ItemSlots[0]);
        if (item)
        {
            weaponDamage = item->Damage + FMath::RandRange(-2, 2);
            weaponDamage = FMath::Max(1, weaponDamage);
        }
    }

    // Total damage before defense
    int rawDamage = Attackerunit->Damage + weaponDamage;
    float damageMultiplier = 1.0f + (static_cast<float>(Attackerunit->Attack) / 10.0f);
    int calculatedDamage = static_cast<int>(rawDamage * damageMultiplier);

    Attackerunit->mutationData->AddExperience(2);
    // Include defender's armor if any
    int armorDefense = 0;
    if (Defenderunit->ItemSlots[1].IsValid())
    {
        AItem* armor = Cast<AItem>(Defenderunit->ItemSlots[1]);
        if (armor)
        {
            armorDefense = armor->Defence;
        }
    }

    int totalDefense = Defenderunit->Defence + armorDefense;
    int finalDamage = FMath::Max(1, calculatedDamage - totalDefense);

    // Mutate check
    if (finalDamage >= Defenderunit->Health && Attackerunit->bIsPlayerUnit)
    {
        Attackerunit->mutationData->AddExperience(Defenderunit->experienceReward);

        GetWorld()->GetTimerManager().SetTimer(
            DelayTimerHandle,
            [WeakAttacker = TWeakObjectPtr<APlayerUnit>(Attackerunit)]()
            {
                if (WeakAttacker.IsValid())
                {
                    WeakAttacker->Mutate();
                }
            },
            1.0f,
            false
        );
    }

    UE_LOG(LogTemp, Warning, TEXT("Dealing %d damage to %s"), finalDamage, *Defenderunit->GetName());

    ApplyKnockback(Attackerunit, Defenderunit);
    HandleUnitDamage(Defenderunit, finalDamage); 
}


void ACombatManager::ApplyKnockback(TWeakObjectPtr<APlayerUnit> Attackerunit, TWeakObjectPtr<APlayerUnit> Defenderunit)
{
    if (!Attackerunit.IsValid() || !Defenderunit.IsValid())
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

void ACombatManager::ResetKnockbackPosition(TWeakObjectPtr<APlayerUnit> Defenderunit)
{
    if (!Defenderunit.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("ResetKnockbackPosition failed: Defender is null!"));
        return;
    }

    // Reset the defender's position to its original tile
    Defenderunit->SetActorLocation(Defenderunit->KnockbackStartPosition);
    Defenderunit->bIsKnockbackActive = false;
}

void ACombatManager::HandleUnitDamage(TWeakObjectPtr<APlayerUnit> unit, int amount)
{
    if (!unit.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("HandleUnitDamage failed: Unit is null!"));
        return;
    }

    unit->Health -= amount;

    if (SB_Impact)
    {
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), SB_Impact, unit->GetActorLocation());
    }

    unit->OnHealthChanged.Broadcast();
    unit->UpdateHealthBar();

    UE_LOG(LogTemp, Warning, TEXT("%s took %d damage"), *unit->GetName(), amount);

    if (unit->Health <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("%s has been killed!"), *unit->GetName());
        KillUnit(unit);
    }
}


void ACombatManager::HealUnit(TWeakObjectPtr<APlayerUnit> unit, int amount)
{
	if (!unit.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("HealUnit failed: Unit is null!"));
		return;
	}
	unit->Health += amount;
	if (unit->Health > unit->maxHealth + unit->MaxHealthFromItems)
	{
		unit->Health = unit->maxHealth + unit->MaxHealthFromItems;
	}
	unit->OnHealthChanged.Broadcast();
	unit->UpdateHealthBar();
	if (SB_Heal) {
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), SB_Heal, unit->GetActorLocation());
	}
}

void ACombatManager::KillUnit(TWeakObjectPtr<APlayerUnit> unit)
{
    if (!unit.IsValid())
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
        GameManager->RemoveUnitFromQueue(unit.Get());

        // If the killed unit was the current turn unit, properly move to the next turn
        if (bWasCurrentUnit)
        {
            GameManager->EndUnitTurn();
        }

        // Optionally update the turn order after a unit dies
        GameManager->UpdateTurnQueue();
    }
    if (unit->bIsPlayerUnit) {
        if (SB_PlayerHurt) {
            UGameplayStatics::PlaySound2D(GetWorld(), SB_PlayerHurt);
        }
    }
    else {
        if (SB_EnemyHurt) {
            UGameplayStatics::PlaySound2D(GetWorld(), SB_EnemyHurt);
        }
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
