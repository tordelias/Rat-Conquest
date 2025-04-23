// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyEnemyAIFrogController.h"
#include "MyBossAI.generated.h"

UCLASS()
class RAT_CONQUEST_API AMyBossAI : public AMyEnemyAIFrogController
{
	GENERATED_BODY()

	void ChooseAction() override;

	void FrogAction();

	void DefaultAction();


	
};
