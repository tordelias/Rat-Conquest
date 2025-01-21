// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Rat_Conquest/Components/InteractionInterface.h"
#include "GridTile.generated.h"


class APlayerUnit;

UCLASS()
class RAT_CONQUEST_API AGridTile : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGridTile();

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* TileMesh;

	bool bIsOccupied;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	bool testTile = false;
	FVector2D GridPosition;


	virtual void BeginFocus() override;
	virtual void EndFocus() override;
	virtual void Interact(APlayerCamera* PlayerCharacter) override;

	UPROPERTY(VisibleInstanceOnly, Category = "Pickup")
	FInteractableData InstanceInteractableData;

	APlayerUnit* unitRefrence;
	void UpdateInteractableData();
	void SetUnitRefrence(APlayerUnit* unit);
	void RemoveUnitRefrence();


	UPROPERTY(VisibleAnywhere, Category = "Occupants")
	TArray<AActor*> tileObjects;

	void AddOccupant(AActor* tileObj);
	void RemoveOccupant(AActor* tileObj);
	bool IsTileOccupied();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
