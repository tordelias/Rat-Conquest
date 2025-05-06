// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Rat_Conquest/Items/ItemBase.h"
#include "GameFramework/Actor.h"
#include "Rat_Conquest/Abilites/AbilityComponent.h" 
#include "Rat_Conquest/Components/InteractionInterface.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Item.generated.h"


UCLASS()
class RAT_CONQUEST_API AItem : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

	void EquipItem(AActor* UnitOwner);
	void DropItem();
	void UseItem();
	void ConsumeItem();



	UFUNCTION(BlueprintCallable, Category = "Item")
	void InitializeItem();

	void initalizeInteractableData(); 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	UStaticMeshComponent* ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	UStaticMeshComponent* RingMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	UNiagaraSystem* ItemEffect;


	UPROPERTY()
	UNiagaraComponent* ActiveEffect;

	UPROPERTY()
	FInteractableData InstanceInteractableData; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	UDataTable* ItemDataTable;
	 //stats

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data")
	FName RowName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	int Damage = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	int Health = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	int Movement = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	int Range = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	int Defence = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	int Attack = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	int Initiative = 0;

	//rotation variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	float YawRotationSpeed = 1.0f;
	//bobbing variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	float BobbingSpeed = 1.0f;

	bool bIsEquipped = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	bool bIsMelee = true;
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	TArray<UAbilityComponent*> GetAbilities() const { return Abilities; }

	void InitializeAbilities();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	UItemBase* ItemDataB;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	TArray<TSubclassOf<UAbilityComponent>> AbilityClasses;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
	TArray<UAbilityComponent*> Abilities;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
