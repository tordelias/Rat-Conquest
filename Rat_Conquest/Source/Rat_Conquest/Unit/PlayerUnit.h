// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Rat_Conquest/Components/InteractionInterface.h"
#include "PlayerUnit.generated.h"

class AGridManager;
class ACombatManager;
class AGridTile;
class AItem;
UCLASS()
class RAT_CONQUEST_API APlayerUnit : public APawn, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayerUnit();
	void MoveToTile(FVector2D NewGridPosition);
	void SetInitalPosition(FVector2D position);
	void DelayedInitalPosition();
	void PlayerAttack(class APlayerCamera* PlayerCharacter);

	void ExecutePlayerTurn();
	void ExecuteAITurn();
	void FinishTurn();
	void DestoryUnit();

	void ResetPosition();

	//item stuff
	void CheckForItems();
	void CalculateStats();
	void DropItem(AItem* OldItem, FVector2D CurrentPosition);
	//should be skeleton mesh
	UPROPERTY(EditAnywhere, Category = "Mesh")
	UStaticMeshComponent* mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	FVector2D CurrentGridPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	AGridManager* GridManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Manager")
	ACombatManager* combatManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	FVector2D GridStartPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	bool bIsPlayerUnit = true;


	//the amount of tiles the unit can move pr turn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	int MovementSpeed = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	int Health = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	int Damage = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	int Defence = 0;

	bool bFirstMove = true;

	TArray<AItem*> ItemSlots; //0 = weapon, 1 = armor, 2 = accessory

	void BeginFocus();
	void EndFocus();
	virtual void BeginMouseHoverFocus() override; 
	virtual void EndMouseHoverFocus() override;
	virtual void Interact(APlayerCamera* PlayerCharacter) override;

private:
	bool bIsMoving = false;                 
	FVector StartPosition;                  
	FVector TargetPosition;  
	
	float MovementProgress = 0.0f;          
	float MovementDuration = 0.5f;

	TArray<AGridTile*> MovedTiles;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void UpdateInteractableData();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleInstanceOnly, Category = "Interact")
	FInteractableData InstanceInteractableData;

	//AI stuff (Should be moved to a AI controller)
	void MoveToGridPosition();
	APlayerUnit* FindEnemyunit();
	void MoveToClosestPossibleTile(APlayerUnit* Enemy);

	void Attack(APlayerUnit* Enemy);

private: 


};
