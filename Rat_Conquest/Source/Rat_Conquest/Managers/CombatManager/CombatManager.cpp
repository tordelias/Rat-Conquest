// Fill out your copyright notice in the Description page of Project Settings.

#include "CombatManager.h"
#include "Rat_Conquest/Player/PlayerCamera.h"
#include "Kismet/GameplayStatics.h"
#include "Rat_Conquest/Unit/PlayerUnit.h"
#include "Rat_Conquest/Managers/GameManager/GameManager.h"
#include "Rat_Conquest/Items/Item.h"

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
	//UE_LOG(LogTemp, Display, TEXT("%f damage"),
	//	Attackerunit->damage);
	if (Attackerunit->ItemSlots[0])
	{
		AItem* item = Cast<AItem>(Attackerunit->ItemSlots[0]);
		if (item) {
			weaponDamage = item->Damage;
		}
		
	}
	int TotalDamage = Attackerunit->Damage + weaponDamage;

	UE_LOG(LogTemp, Warning, TEXT("Dealing %d damage to %s"), TotalDamage, *Defenderunit->GetName());

	TakeDamage(Defenderunit, TotalDamage);
}

void ACombatManager::TakeDamage(APlayerUnit* unit, int amount)
{
	if (!unit)
	{
		UE_LOG(LogTemp, Error, TEXT("TakeDamage failed: Unit is null!"));
		return;
	}
	int TotalDamage = FMath::Max(amount - unit->Defence, 1);
	unit->Health -= TotalDamage;

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

	// If there's a player character, clear the current unit reference
	APlayerCamera* PlayerCharacter = Cast<APlayerCamera>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (PlayerCharacter)
	{
		PlayerCharacter->SetCurrentUnit(nullptr);
	}
	AGameManager* GameManager = Cast<AGameManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameManager::StaticClass()));
	if (GameManager)
	{
		GameManager->RemoveUnitFromQueue(unit);
		GameManager->EndUnitTurn();
	}
	unit->DestoryUnit();
	// Destroy the unit
	unit->Destroy();
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
