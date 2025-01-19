// Fill out your copyright notice in the Description page of Project Settings.

#include "CombatManager.h"
#include "Rat_Conquest/Unit/PlayerUnit.h"

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

	//UE_LOG(LogTemp, Display, TEXT("%f damage"),
	//	Attackerunit->damage);

	TakeDamage(Defenderunit, Attackerunit->damage);
}

void ACombatManager::TakeDamage(APlayerUnit* unit, float amount)
{
	if (!unit)
	{
		UE_LOG(LogTemp, Error, TEXT("TakeDamage failed: Unit is null!"));
		return;
	}

	unit->health -= amount;

	//UE_LOG(LogTemp, Warning, TEXT("took %f damage. Remaining health: %f", ),amount, unit->health);

	if (unit->health <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("has been killed!"));
		KillUnit(unit);
	}
	else
	{
		//UE_LOG(LogTemp, Display, TEXT(" survived with %f health remaining"), unit->health);
	}
}

void ACombatManager::KillUnit(APlayerUnit* unit)
{
	if (!unit)
	{
		UE_LOG(LogTemp, Error, TEXT("KillUnit failed: Unit is null!"));
		return;
	}

	

	if (unit->IsA(AActor::StaticClass()))
	{
		unit->Destroy();
		
	}
	else
	{
		
	}
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
