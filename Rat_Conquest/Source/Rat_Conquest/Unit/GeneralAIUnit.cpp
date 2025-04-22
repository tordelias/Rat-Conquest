// Fill out your copyright notice in the Description page of Project Settings.


#include "GeneralAIUnit.h"
#include "NiagaraFunctionLibrary.h"

AGeneralAIUnit::AGeneralAIUnit()
{
	TongueWorldTarget = FVector::ZeroVector;
	animationToPlay = FVector2D(0, 75);
}

void AGeneralAIUnit::RandomizeStats()
{
	MovementSpeed = FMath::RandRange(2, 5);
	Damage = FMath::RandRange(1, 5);
	Defence = FMath::RandRange(0, 5);
	maxHealth = FMath::RandRange(5, 10);
	Health = maxHealth;
	Attack = FMath::RandRange(1, 5);
	AttackRange = FMath::RandRange(4, 8);
	Initiative = FMath::RandRange(1, 5);

	if (IsFrog)
	{
		int randomTexture = FMath::RandRange(0, 3);
		SetTexcture(randomTexture);
		SetStatsBasedOnColorID(randomTexture);
	}
}

void AGeneralAIUnit::SetTexcture(int num)
{
	if (UnitMaterials.IsValidIndex(num))
	{
		UMaterialInterface* Material = UnitMaterials[num];
		if (Material)
		{
			SkeletalMesh->SetMaterial(0, Material);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Material is NULL!"));
		}
		if (UnitIcons.IsValidIndex(num))
		{
			UnitIcon = UnitIcons[num];
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Invalid icon index!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid material index!"));
	}
}

void AGeneralAIUnit::SetStatsBasedOnColorID(int32 FrogColorID)
{
	// Base stats
	float mHealth = FMath::RandRange(10.f, 15.f);
	float Defense = FMath::RandRange(2.f, 5.f);
	float Speed = FMath::RandRange(2.f, 5.f);
	float damage = FMath::RandRange(2.f, 3.f);
	float initiative = FMath::RandRange(1.f, 3.f);

	switch (FrogColorID)
	{
	case 1: // Green - Normal
		// No changes
		break;

	case 2: // Brown - +Health, +Defense, -Speed, -Damage
		mHealth += 15.f;
		Defense += 5.f;
		Speed -= 2.f;
		damage -= 3.f;
		break;

	case 0: // Purple - +Damage, -Health //bodyslam
		damage += 5.f;
		mHealth -= 10.f;
		break;

	case 3: // Blue - +Speed, -Health, +Initiative //bodyslam
		Speed += 5.f;
		mHealth -= 7.f;
		initiative += 5.f;
		break;

	default:
		UE_LOG(LogTemp, Warning, TEXT("Invalid FrogColorID passed: %d"), FrogColorID);
		break;
	}

	// Clamp to prevent stats from dropping too low
	mHealth = FMath::Max(5.f, mHealth);
	Defense = FMath::Max(0.f, Defense);
	Speed = FMath::Max(2.f, Speed);
	damage = FMath::Max(1.f, damage);
	initiative = FMath::Max(1.f, initiative);

	// Apply to unit
	maxHealth = mHealth;
	Health = mHealth;
	Defence = Defense;
	MovementSpeed = Speed;
	Damage = damage;
	Initiative = initiative;

	UE_LOG(LogTemp, Log, TEXT("Stats set for FrogColorID %d"), FrogColorID);
}

void AGeneralAIUnit::SpawnVFX()
{
	if (BodySlamVFX)
	{
		FVector SpawnLocation = GetActorLocation();
		//SpawnLocation.Z = 6.0f; // Adjust Z position if needed
		FRotator SpawnRotation = GetActorRotation();
		FVector Scale = FVector(10.0f);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BodySlamVFX, SpawnLocation, SpawnRotation, Scale);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BodySlamVFX is null!"));
	}
}