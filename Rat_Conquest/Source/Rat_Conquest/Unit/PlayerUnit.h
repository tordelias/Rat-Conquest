// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Rat_Conquest/Components/InteractionInterface.h"
#include "PlayerUnit.generated.h"

class AGridManager;

UCLASS()
class RAT_CONQUEST_API APlayerUnit : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayerUnit();
	void MoveToTile(FVector2D NewGridPosition);
	void SetInitalPosition(FVector2D position);
	void DelayedInitalPosition();

	void ExecutePlayerTurn();
	void ExecuteAITurn();
	void FinishTurn();

	//should be skeleton mesh
	UPROPERTY(EditAnywhere, Category = "Mesh")
	UStaticMeshComponent* mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	FVector2D CurrentGridPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	AGridManager* GridManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	bool bIsPlayerUnit = true;


	//the amount of tiles the unit can move pr turn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	int movementSpeed = 3;

	bool bFirstMove = true;


	virtual void BeginFocus() override;
	virtual void EndFocus() override;
	virtual void Interact(APlayerCamera* PlayerCharacter) override;

private:
	bool bIsMoving = false;                 
	FVector StartPosition;                  
	FVector TargetPosition;                 
	float MovementProgress = 0.0f;          
	float MovementDuration = 0.5f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleInstanceOnly, Category = "Interact")
	FInteractableData InstanceInteractableData;

	void UpdateInteractableData();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
