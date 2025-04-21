// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableGridObject.generated.h"

class APlayerUnit;
class ACombatManager;
class AGameManager;


UENUM(BlueprintType)
enum class EObjectType : uint8
{
	None,
	Healing,
	ExplosiveBarrel,
	Battery,
	Tunnel,
	Tower,
	CagedRat,

};

UCLASS()
class RAT_CONQUEST_API AInteractableGridObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractableGridObject();

	void InteractWithObject(TWeakObjectPtr<APlayerUnit> InteractingUnit);

	void Heal();

	void Explode();

	void DealAoEDamage();

	void MovePlayer();

	void GetCagedRat();

	void DestroyObject();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object Properties")
	EObjectType ObjectType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object Properties")
	UStaticMeshComponent* ObjectMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object Properties")
	int HealingAmount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object Properties")
	bool bIsConsumable = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Manager")
	TWeakObjectPtr<ACombatManager> CombatManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Manager")
	TWeakObjectPtr<AGameManager> GameManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit")
	TWeakObjectPtr<APlayerUnit> unitRefrence;
};
