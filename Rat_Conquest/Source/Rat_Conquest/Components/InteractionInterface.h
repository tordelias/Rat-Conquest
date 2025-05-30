// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractionInterface.generated.h"

class APlayerCamera;

UENUM()
enum class EInteractionType : uint8
{
	IT_Pickup UMETA(DisplayName = "Pickup"),
	IT_Interact UMETA(DisplayName = "Interact"),
	IT_Enemy UMETA(DisplayName = "Enemy"), 
	IT_GridCell UMETA(DisplayName = "GridCell")

};

USTRUCT()
struct FInteractableData
{
	GENERATED_USTRUCT_BODY()


	FInteractableData() : InteractableType(EInteractionType::IT_Pickup), UnitName(FText::FromString("")), UnitHealth(0), UnitDamage(0), UnitMovementSpeed(0), Defense(0), Attack(0), Range(0), Initative(0), maxHealth(0), InteractionDuration(0.0f)
	{
	};
	UPROPERTY(EditInstanceOnly)
	EInteractionType InteractableType;

	UPROPERTY(EditInstanceOnly)
	FText UnitName;

	UPROPERTY(EditInstanceOnly)
	uint32 UnitHealth;

	UPROPERTY(EditInstanceOnly)
	uint32 UnitDamage;

	UPROPERTY(EditInstanceOnly)
	uint32 UnitMovementSpeed;

	UPROPERTY(EditInstanceOnly)
	uint32 Defense;

	UPROPERTY(EditInstanceOnly)
	uint32 Attack;

	UPROPERTY(EditInstanceOnly)
	uint32 Range;

	UPROPERTY(EditInstanceOnly)
	uint32 Initative;

	UPROPERTY(EditInstanceOnly)
	uint32 maxHealth;

	UPROPERTY(EditInstanceOnly)
	uint32 MaxDamage;

	UPROPERTY(EditInstanceOnly)
	uint32 MinDamage;

	UPROPERTY(EditInstanceOnly)
	float InteractionDuration;
};


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RAT_CONQUEST_API IInteractionInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual void BeginMouseHoverFocus();
	virtual void EndMouseHoverFocus();
	virtual void BeginInteract();
	virtual void EndInteract();
	virtual void Interact(TWeakObjectPtr<APlayerCamera> PlayerCharacter);

	FInteractableData InteractableData;
};
