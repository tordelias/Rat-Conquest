// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GeneralAIUnit.h"
#include "MyBossUnit.generated.h"

UENUM(BlueprintType)
enum class EBossType : uint8
{
	Frog UMETA(DisplayName = "Frog"),
	IronRat UMETA(DisplayName = "IronRat"),
	Mole UMETA(DisplayName = "Mole"),
	PlayerRat UMETA(DisplayName = "PlayerRat")
};

UCLASS()
class RAT_CONQUEST_API AMyBossUnit : public AGeneralAIUnit
{
	GENERATED_BODY()


public:
	void RandomizeStats() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	EBossType BossType = EBossType::Frog;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	int32 BossID = 0;

	FText BossName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	TArray<FText> BossNames;
	void UpdateHealthBar() override;


private:
	void FrogStats();
	void IronRatStats();
	void MoleStats();
	void PlayerRatStats();

	void InitalizeHealthBar();


	
};
