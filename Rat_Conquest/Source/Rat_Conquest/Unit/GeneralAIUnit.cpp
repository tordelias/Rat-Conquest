// Fill out your copyright notice in the Description page of Project Settings.


#include "GeneralAIUnit.h"

AGeneralAIUnit::AGeneralAIUnit()
{
}

void AGeneralAIUnit::RandomizeStats()
{
	MovementSpeed = FMath::RandRange(2, 5);
	Damage = FMath::RandRange(1, 5);
	Defence = FMath::RandRange(0, 5);
	maxHealth = FMath::RandRange(5, 10);
	Health = maxHealth;
	Attack = FMath::RandRange(1, 5);
	AttackRange = FMath::RandRange(4, 8);
	Initiative = FMath::RandRange(1, 5);
}
