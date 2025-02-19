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
class AGenericProjectile;

DECLARE_DELEGATE(FOnMovementCompleteSignature);


UCLASS()
class RAT_CONQUEST_API APlayerUnit : public APawn, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayerUnit();
	void MoveToTile(FVector2D NewGridPosition);
	bool CalculatePathToTile(FVector2D InTargetGridPosition);
	TArray<FVector2D> GetPathToTile(FVector2D InTargetGridPosition, FVector2D StartTile);
	void SetInitalPosition(FVector2D position);
	void DelayedInitalPosition();
	void PlayerAttack(class APlayerCamera* PlayerCharacter);
	FVector2D GetCardinalDirection(FVector2D FromGridPos, FVector2D ToGridPos);
	void AttackAfterMovement();
	void ShootProjectile(FVector _EnemyLocation);


	FOnMovementCompleteSignature OnMovementComplete;
	APlayerUnit* EnemyToAttack;

	void ExecutePlayerTurn();
	void ExecuteAITurn();
	void FinishTurn();
	void DestoryUnit();

	void ResetPosition();
	void EquipStartWeapon(AItem* ItemToAdd);
	float ChebyshevDistance(FVector2D A, FVector2D B);


	//item stuff
	void CheckForItems();
	void CalculateStats();
	void DropItem(AItem* OldItem, FVector2D CurrentPosition);
	//should be skeleton mesh
	UPROPERTY(EditAnywhere, Category = "Mesh")
	UStaticMeshComponent* mesh;
	//Skeleton mesh
	UPROPERTY(EditAnywhere, Category = "Mesh")
	USkeletalMeshComponent* SkeletalMesh;

	//UnitIcon
	UPROPERTY(EditAnywhere, Category = "Icon")
	UTexture2D* UnitIcon;
	bool bHasWeapon = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	FVector2D CurrentGridPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	AGridManager* GridManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Manager")
	ACombatManager* combatManager;

	//Optional start item to spawn with
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	TSubclassOf<AItem> StartWeapon;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	FVector2D GridStartPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	bool bIsPlayerUnit = true;


	bool bIsCurrentUnit = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	bool bIsRangedUnit = true;

	//the amount of tiles the unit can move pr turn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	int MovementSpeed = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	int Health = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	int Damage = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	int Defence = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	int AttackRange = 6;
	bool bFirstMove = true;

	

	TArray<AItem*> ItemSlots; //0 = weapon, 1 = armor, 2 = accessory

	void BeginFocus();
	void EndFocus();
	virtual void BeginMouseHoverFocus() override; 
	virtual void EndMouseHoverFocus() override;
	virtual void Interact(APlayerCamera* PlayerCharacter) override;

	FVector TargetPosition;
	FVector2D TargetGridPosition;
	UPROPERTY(EditAnywhere, Category = "Projectile")
	TSubclassOf<AGenericProjectile> ProjectileClass;

private:
	bool bIsMoving = false;                 
	FVector StartPosition;                  
	
	float MovementProgress = 0.0f;          
	float MovementDuration = 0.5f;

	TArray<AGridTile*> MovedTiles;
	TArray<AGridTile*> HoverTiles;

	TArray<FVector2D> PathToTake;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void UpdateInteractableData();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FVector2D GetMousePosition(FVector WorldLocation, FVector WorldDirection);

	UPROPERTY(VisibleInstanceOnly, Category = "Interact")
	FInteractableData InstanceInteractableData;

	UStaticMeshComponent* GetMesh() { return mesh; }

	//temp
	FVector KnockbackStartPosition; 
	bool bIsKnockbackActive; 
	float KnockbackProgress;
	float KnockbackDuration = 1.f;
	FVector KnockbackTargetPosition;

	FVector2D TargetEnemyLocation = FVector2D(0,0);
	FVector2D MouseGridPos = FVector2D(0, 0);

private: 


};
