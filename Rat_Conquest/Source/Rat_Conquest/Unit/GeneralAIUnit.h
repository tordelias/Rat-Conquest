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

public:

	AGeneralAIUnit();

	void SetTexcture(int num);

	void SetStatsBasedOnColorID(int32 FrogColorID);


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	bool IsFrog = false; 
	//Niagara VFX
	UPROPERTY(EditAnywhere, Category = "VFX")
	class UNiagaraSystem* BodySlamVFX;

	void SpawnVFX();
	void RandomizeStats() override;

	int AIDifficulty = 5; // 0 = Easy, 1 = Medium, 2 = Hard

	UPROPERTY(BlueprintReadWrite)
	FVector TongueWorldTarget;

	UPROPERTY(BlueprintReadWrite)
	bool ToungeState = false;

	bool bIsSlowFrog = false; 

	void SetToungeWorldTarget(FVector Target) { TongueWorldTarget = Target; OnBoneMoved.Broadcast(); }


	UPROPERTY(BlueprintAssignable, Category = "Animation")
	FOnBlendSpaceChange OnBoneMoved;

	
};
