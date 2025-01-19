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
		return;

	TakeDamage(Defenderunit, Attackerunit->damage);
}

void ACombatManager::TakeDamage(APlayerUnit* unit, float amount)
{
	if (!unit)
		return;

	unit->health -= amount;

	//UE_LOG(LogTemp, Display, TEXT("%s took %f damage. Remaining health: %f"), *unit->GetName(), amount, unit->health);

	if (unit->health <= 0)
	{
		KillUnit(unit);
	}
}

void ACombatManager::KillUnit(APlayerUnit* unit)
{
	if (!unit)
		return;

	if (unit->IsA(AActor::StaticClass()))
	{
		unit->Destroy();
	}
}

// Called when the game starts or when spawned
void ACombatManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACombatManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

