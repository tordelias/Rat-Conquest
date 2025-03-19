// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableGridObject.h"
#include "Rat_Conquest/Managers/GridManager/GridManager.h"
#include "Rat_Conquest/Unit/PlayerUnit.h"

// Sets default values
AInteractableGridObject::AInteractableGridObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AInteractableGridObject::InteractWithObject()
{
	switch (ObjectType)
	{
	case EObjectType::Healing:
		// Heal the player
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

// Called when the game starts or when spawned
void AInteractableGridObject::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInteractableGridObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

