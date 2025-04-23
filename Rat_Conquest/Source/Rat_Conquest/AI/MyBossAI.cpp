// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBossAI.h"
#include "Rat_Conquest/Unit/MyBossUnit.h"

void AMyBossAI::ChooseAction()
{
	this->Target = nullptr;
	switch (Difficulty)
	{
	case EEnemyAIDifficulty::Easy:
		this->Target = FindClosestEnemy();
		break;
	case EEnemyAIDifficulty::Normal:
		this->Target = FindEnemyByThreat();
		break;
	case EEnemyAIDifficulty::Hard:
		this->Target = FindMostVulnerableEnemy();
		break;
	}
	//Log the target
	if (this->Target.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Target found: %s"), *this->Target->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No target found"));
		return;
	}

	AMyBossUnit* AI = Cast<AMyBossUnit>(GetPawn());
	switch (AI->BossType)
	{
	case EBossType::Frog:
		//Frog logic
		FrogAction();
		break;
	case EBossType::IronRat:
		// IronRat Logic
		DefaultAction();
		break;
	case EBossType::Mole:
		// Mole logic
		DefaultAction();
		break;
	case EBossType::PlayerRat:
		// PlayerRat logic
		DefaultAction();
		break;
	default:
		DefaultAction();
		break;
	}
}

void AMyBossAI::FrogAction()
{
	AGeneralAIUnit* AI = Cast<AGeneralAIUnit>(GetPawn());
	if (!AI) return;
	if (AI->ChebyshevDistance(AI->CurrentGridPosition, Target->CurrentGridPosition) >= AI->MovementSpeed && FMath::RandRange(0, 1) == 0)
	{
		ToungeGrab();
		return;
	}
	//25% chance to use body slam
	if (FMath::RandRange(0, 3) == 0)
	{
		StartBodySlam();
		return;
	}
	else
	{
		MeleeAttack();
	}
}

void AMyBossAI::DefaultAction()
{
	if (bIsRanged)
	{
		RangedAttack();
	}
	else
	{
		MeleeAttack();
	}
}
