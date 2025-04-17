// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Rat_Conquest/Unit/PlayerUnit.h"

AWeapon::AWeapon()
{

}

void AWeapon::UseItem()
{
	UE_LOG(LogTemp, Warning, TEXT("Weapon Used"));
    switch (WeaponAbility)
    {
    case EWeaponAbility::Slash:
        UseSlash();
        break;
    case EWeaponAbility::ArrowShot:
        UseArrowShot();
        break;
	case EWeaponAbility::CrossBowShot:
		UseCrossBowShot();
		break;
	case EWeaponAbility::MusketShot:
		break;
    default:
        UE_LOG(LogTemp, Warning, TEXT("No ability defined for this weapon."));
        break;
    }

}

void AWeapon::UseSlash()
{
	UE_LOG(LogTemp, Warning, TEXT("Slash Used"));
}

void AWeapon::UseArrowShot()
{
	UE_LOG(LogTemp, Warning, TEXT("Arrow Shot Used"));
	if (ProjectileClass && this != nullptr)
	{
		// Spawn the arrows
		FVector SpawnLocation = GetActorLocation(); // Start at the unit's location
		FVector EnemyLocation = _EnemyLocation; // Target location (e.g., enemy position)

		FRotator SpawnRotation = (EnemyLocation - SpawnLocation).Rotation();

		AGenericProjectile* Projectile = GetWorld()->SpawnActor<AGenericProjectile>(
			ProjectileClass, SpawnLocation, SpawnRotation);

		if (Projectile)
		{
			// Initialize the projectile with a curve
			Projectile->InitializeProjectileWithCurve(SpawnLocation, EnemyLocation);
			if (UnitRef.IsValid() && EnemyRef.IsValid())
			{
				// Set the projectile's damage and unit reference
				Projectile->SetProjectileUnitRefrence(UnitRef, EnemyRef);
			}
			
			ProjectileTime = Projectile->TimeToTarget;
		}
	}
}

void AWeapon::UseCrossBowShot()
{
	if (ProjectileClass && this != nullptr)
	{
		// Spawn the arrows
		FVector SpawnLocation = GetActorLocation(); // Start at the unit's location
		FVector EnemyLocation = _EnemyLocation; // Target location (e.g., enemy position)

		FRotator SpawnRotation = (EnemyLocation - SpawnLocation).Rotation();

		AGenericProjectile* Projectile = GetWorld()->SpawnActor<AGenericProjectile>(
			ProjectileClass, SpawnLocation, SpawnRotation);

		if (Projectile)
		{
			// Initialize the projectile with a curve

			Projectile->InitializeProjectileWithStraightPath(SpawnLocation, EnemyLocation);
			if (UnitRef.IsValid() && EnemyRef.IsValid())
			{
				// Set the projectile's damage and unit reference
				Projectile->SetProjectileUnitRefrence(UnitRef, EnemyRef);
			}
			ProjectileTime = Projectile->TimeToTarget;
		}
	}
}

void AWeapon::UseMusketShot()
{
}

void AWeapon::SetEnemyLocation(FVector CurrentLocation)
{
	_EnemyLocation = CurrentLocation;
	
}

void AWeapon::SetUnitRefrence(TWeakObjectPtr<APlayerUnit> _UnitRef, TWeakObjectPtr<APlayerUnit> _EnemyRef)
{
	UnitRef = _UnitRef;
	EnemyRef = _EnemyRef;

}
