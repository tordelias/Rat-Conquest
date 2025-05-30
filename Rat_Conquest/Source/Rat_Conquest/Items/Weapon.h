// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Rat_Conquest/Projectiles/GenericProjectile.h"
#include "Weapon.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EWeaponAbility : uint8
{
    None,
    Slash,
    ArrowShot,
	HammerSlam,
	CrossBowShot,
	MusketShot
  
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

	void UseCrossBowShot();

	void UseMusketShot();

	void SetEnemyLocation(FVector CurrentLocation);

	void SetUnitRefrence(TWeakObjectPtr<APlayerUnit> _UnitRef, TWeakObjectPtr<APlayerUnit> _EnemyRef);
	
	FVector _EnemyLocation;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	TSubclassOf<AGenericProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit")
	TWeakObjectPtr<APlayerUnit> UnitRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit")
	TWeakObjectPtr<APlayerUnit> EnemyRef;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties")
	EWeaponAbility WeaponAbility;
	
	float ProjectileTime = 1.0f;
};
