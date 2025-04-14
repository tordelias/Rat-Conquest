// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerUnit.h"
#include "GeneralAIUnit.generated.h"

/**
 * 
 */
UCLASS()
class RAT_CONQUEST_API AGeneralAIUnit : public APlayerUnit
{
	GENERATED_BODY()

	AGeneralAIUnit();

	void RandomizeStats() override;
	void SetTexcture(int num);

public: 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	bool IsFrog = false; 



	
};
