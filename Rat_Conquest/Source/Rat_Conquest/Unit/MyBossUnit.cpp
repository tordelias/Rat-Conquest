// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBossUnit.h"
#include "Rat_Conquest/Widgets/MainHUD.h"

void AMyBossUnit::RandomizeStats()
{
	switch (BossType)
	{
	case EBossType::Frog:
		FrogStats();
		break;
	case EBossType::IronRat:
		IronRatStats();
		break;
	case EBossType::Mole:
		MoleStats();
		break;
	case EBossType::PlayerRat:
		PlayerRatStats();
		break;
	default:
		break;
	}
	//Random name from array
	int randomName = FMath::RandRange(0, BossNames.Num() - 1);
	BossName = BossNames[randomName];

	AMainHUD* hud = Cast<AMainHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
	if (hud)
	{
		hud->ShowBossHealthBar(BossName);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("HUD is null |MyBossUnit.cpp|"));
	}

}

void AMyBossUnit::FrogStats() //Health
{// Base stats
	float mHealth = 50.f;
	float Defense = FMath::RandRange(2.f, 4.f);
	float Speed = 3.f;
	float damage = FMath::RandRange(2.f, 3.f);
	float initiative = FMath::RandRange(1.f, 3.f);
	// Set stats to unit
	maxHealth = mHealth;
	Health = mHealth;
	Defence = Defense;
	MovementSpeed = Speed;
	Damage = damage;
	Initiative = initiative;
}

void AMyBossUnit::IronRatStats() //Damage
{
	// Base stats
	float mHealth = 20.f;
	float Defense = 0.f; 
	float Speed = FMath::RandRange(3.f, 5.f);
	float damage = 10.f;
	float initiative = FMath::RandRange(1.f, 3.f);
	// Set stats to unit
	maxHealth = mHealth;
	Health = mHealth;
	Defence = Defense;
	MovementSpeed = Speed;
	Damage = damage;
	Initiative = initiative;
}

void AMyBossUnit::MoleStats() //Initative
{
	// Base stats
	float mHealth = 30.f;
	float Defense = FMath::RandRange(2.f, 4.f);
	float Speed = FMath::RandRange(2.f, 5.f);
	float damage = FMath::RandRange(2.f, 3.f);
	float initiative = 20; 
	// Set stats to unit
	maxHealth = mHealth;
	Health = mHealth;
	Defence = Defense;
	MovementSpeed = Speed;
	Damage = damage;
	Initiative = initiative;
}

void AMyBossUnit::PlayerRatStats() //Speed
{
	// Base stats
	float mHealth = 25.f;
	float Defense = FMath::RandRange(2.f, 5.f);
	float Speed = 20.f; 
	float damage = FMath::RandRange(2.f, 3.f);
	float initiative = FMath::RandRange(1.f, 3.f);
	// Set stats to unit
	maxHealth = mHealth;
	Health = mHealth;
	Defence = Defense;
	MovementSpeed = Speed;
	Damage = damage;
	Initiative = initiative;
}

void AMyBossUnit::UpdateHealthBar()
{
	//cast to mainHUD
	AMainHUD* hud = Cast<AMainHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
	if (hud)
	{
		hud->SetHealthBarPercentage(Health / maxHealth);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("HUD is null |MyBossUnit.cpp|"));
	}
}
