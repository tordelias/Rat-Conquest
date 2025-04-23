// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyAIController.h"
#include "MyEnemyAIFrogController.generated.h"

USTRUCT()
struct FKnockbackLerpData
{
	GENERATED_BODY()

	TWeakObjectPtr<APlayerUnit> Unit;
	FVector StartLocation;
	FVector TargetLocation;
	FVector2D TargetGridPosition;
	float Alpha;

	FKnockbackLerpData()
		: Unit(nullptr), StartLocation(FVector::ZeroVector), TargetLocation(FVector::ZeroVector), Alpha(0.0f)
	{
	}
};

USTRUCT()
struct FTonguePullData
{
	GENERATED_BODY()

	TWeakObjectPtr<APlayerUnit> Unit;
	FVector StartLocation;
	FVector TargetLocation;
	FVector2D TargetGridPosition;
	float Alpha;

	FVector* BoneTargetPtr = nullptr;
};

UCLASS()
class RAT_CONQUEST_API AMyEnemyAIFrogController : public AEnemyAIController
{
	GENERATED_BODY()


	void ChooseAction() override;

	virtual void Tick(float DeltaTime) override;

	TArray<FKnockbackLerpData> KnockbackUnits;
	UPROPERTY()
	TArray<FTonguePullData> TonguePullUnits;

	float KnockbackDuration = 0.3f;

	FVector BoneLocation;
	FVector BoneDirection;
public:
	void StartBodySlam();
	void BodySlam();
	void ToungeGrab();
	
};
