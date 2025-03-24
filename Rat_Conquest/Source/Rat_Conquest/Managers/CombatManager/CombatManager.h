// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CombatManager.generated.h"


class APlayerUnit;
UCLASS()
class RAT_CONQUEST_API ACombatManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACombatManager();
	void DealDamageToUnit(APlayerUnit* Attackerunit, APlayerUnit* Defenderunit);
	void ApplyKnockback(APlayerUnit* Attackerunit, APlayerUnit* Defenderunit);
	void ResetKnockbackPosition(APlayerUnit* Defenderunit);
	void HandleUnitDamage(APlayerUnit* unit, int amount);
	void KillUnit(APlayerUnit* unit);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound & Effects")
	USoundBase* SB_PlayerHurt;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound & Effects")
	USoundBase* SB_EnemyHurt;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound & Effects")
	USoundBase* SB_Impact;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
