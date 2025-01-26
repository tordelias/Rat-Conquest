// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EWeaponAbility : uint8
{
    None,
    Slash,
    ArrowShot
  
};

UCLASS()
class RAT_CONQUEST_API AWeapon : public AItem
{
	GENERATED_BODY()
public:
	AWeapon();

	virtual void UseItem();

	void UseSlash();

	void UseArrowShot();



	


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties")
	EWeaponAbility WeaponAbility;
	
};
