// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

class APlayerUnit; 

/**
 * 
 */
UCLASS()
class RAT_CONQUEST_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;

	void MoveToGridPosition();
	APlayerUnit* FindEnemyunit();
	void MoveToClosestPossibleTile(APlayerUnit* Enemy);

	void Attack(APlayerUnit* Enemy);
	void RandedAttack(APlayerUnit* Enemy);
	
};
