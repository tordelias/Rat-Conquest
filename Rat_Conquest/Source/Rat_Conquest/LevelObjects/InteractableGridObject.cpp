// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableGridObject.h"
#include "Rat_Conquest/Managers/GridManager/GridManager.h"
#include "Rat_Conquest/Unit/PlayerUnit.h"
#include "Rat_Conquest/Managers/CombatManager/CombatManager.h"
#include "Rat_Conquest/Managers/GameManager/GameManager.h"
#include "Rat_Conquest/Unit/PlayerUnit.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
AInteractableGridObject::AInteractableGridObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ObjectMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ObjectMesh"));
	RootComponent = ObjectMesh;

}

void AInteractableGridObject::InteractWithObject(TWeakObjectPtr<APlayerUnit> InteractingUnit)
{
	unitRefrence = InteractingUnit;
	switch (ObjectType)
	{
	case EObjectType::Healing:
		Heal();
		break;
	case EObjectType::ExplosiveBarrel:
		// Explode and deal damage to enemies
		break;
	case EObjectType::Battery:
		// Deal aoE damage to enemies
		break;
	case EObjectType::Tunnel:
		// Move the player to another spot on the map
		break;
	case EObjectType::Tower:
		// Get better range
		break;
	default:
		break;
	}

}

void AInteractableGridObject::Heal()
{
	if (!unitRefrence.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Unit reference is invalid!"));
		return;
	}
	HealingAmount = 10; // Example healing amount
	CombatManager->HealUnit(unitRefrence, HealingAmount);
	UE_LOG(LogTemp, Warning, TEXT("Healing %d health"), HealingAmount);
	UE_LOG(LogTemp, Warning, TEXT("Used healing mushroom"), HealingAmount);
	if (bIsConsumable) {
		DestroyObject();
	}
}

void AInteractableGridObject::Explode()
{
}

void AInteractableGridObject::DealAoEDamage()
{
}

void AInteractableGridObject::MovePlayer()
{
}

void AInteractableGridObject::GetCagedRat()
{
	// Get the player unit and move it to a new position
	if (unitRefrence.IsValid() && GameManager.IsValid())
	{
		FVector2D GridPositon = unitRefrence->CurrentGridPosition; // Example new position
		GameManager->SpawnNewPlayerUnit(GridPositon);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Unit reference is invalid!"));
	}
}

void AInteractableGridObject::DestroyObject()
{
	// Destroy the object
	Destroy();
	// Optionally, you can also remove it from the grid manager if needed
	
	
}

// Called when the game starts or when spawned
void AInteractableGridObject::BeginPlay()
{
	Super::BeginPlay();
	TArray<AActor*> FoundCombatManagers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACombatManager::StaticClass(), FoundCombatManagers);

	if (FoundCombatManagers.Num() > 0)
	{
		CombatManager = Cast<ACombatManager>(FoundCombatManagers[0]);
		if (FoundCombatManagers.Num() > 1)
		{
			UE_LOG(LogTemp, Warning, TEXT("Multiple Combatmanagers found! Using first instance."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No CombatManager found in the level!"));
	}
	TArray<AActor*> FoundGameManagers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGameManager::StaticClass(), FoundGameManagers);
	if (FoundGameManagers.Num() > 0)
	{
		GameManager = Cast<AGameManager>(FoundGameManagers[0]);
		if (FoundGameManagers.Num() > 1)
		{
			UE_LOG(LogTemp, Warning, TEXT("Multiple GameManagers found! Using first instance."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No GameManager found in the level!"));
	}

}

// Called every frame
void AInteractableGridObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

