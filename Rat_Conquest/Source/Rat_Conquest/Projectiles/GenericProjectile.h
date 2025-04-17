// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GenericProjectile.generated.h"

class ACombatManager;
class APlayerUnit;

UCLASS()
class RAT_CONQUEST_API AGenericProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGenericProjectile();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	float Speed = 200.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* CollisionComponent;

	// Projectile mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Manager")
	TWeakObjectPtr<ACombatManager> CombatManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit")
	TWeakObjectPtr<APlayerUnit> UnitRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit")
	TWeakObjectPtr<APlayerUnit> EnemyRef;

	FVector Direction;
	// Target location for the projectile
	FVector TargetLocation;
	FVector StartLocation;

	// Curvature amplitude (higher values mean a bigger curve)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	float CurvatureAmplitude = 2500.0f;

	// Time to reach the target (affects speed)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	float TimeToTarget = 0.750f;

	// Internal timer to control the curve
	float ElapsedTime = 0.0f;

	int damage = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	bool bUseStraightPath = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	bool bUseCurvePath = true;

	void InitializeProjectileWithCurve(const FVector& _StartLocation, const FVector& Target);
	void InitializeProjectileWithStraightPath(const FVector& _StartLocation, const FVector& Target);
	void InitializeProjectile(const FVector& ShootDirection);
	void SetProjectileUnitRefrence(TWeakObjectPtr<APlayerUnit> _unitref, TWeakObjectPtr<APlayerUnit> _enemy);
	void StraightShot();
	void DealDamage();


	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
