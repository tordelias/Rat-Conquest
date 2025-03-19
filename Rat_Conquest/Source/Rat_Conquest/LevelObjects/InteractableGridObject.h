// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableGridObject.generated.h"


UENUM(BlueprintType)
enum class EObjectType : uint8
{
	None,
	Healing,
	ExplosiveBarrel,
	Battery,
	Tunnel,
	Tower

};

UCLASS()
class RAT_CONQUEST_API AInteractableGridObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractableGridObject();

	void InteractWithObject();

	void Heal();

	void Explode();

	void DealAoEDamage();

	void MovePlayer();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object Properties")
	EObjectType ObjectType;
};
