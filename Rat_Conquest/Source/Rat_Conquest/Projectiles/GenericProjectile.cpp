// Fill out your copyright notice in the Description page of Project Settings.
#include "GenericProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Rat_Conquest/Managers/CombatManager/CombatManager.h" 
#include "Rat_Conquest/Unit/PlayerUnit.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AGenericProjectile::AGenericProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	
    PrimaryActorTick.bCanEverTick = true;

    // Create collision component
    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    CollisionComponent->InitSphereRadius(15.0f);
    CollisionComponent->SetCollisionProfileName(TEXT("Projectile"));
    CollisionComponent->OnComponentHit.AddDynamic(this, &AGenericProjectile::OnHit);
    RootComponent = CollisionComponent;

    // Create projectile mesh component
    ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
    ProjectileMesh->SetupAttachment(CollisionComponent);

    // Set defaults
    StartLocation = GetActorLocation();

}

void AGenericProjectile::InitializeProjectileWithCurve(const FVector& _StartLocation, const FVector& Target)
{
    SetActorLocation(_StartLocation);
    TargetLocation = Target + FVector(0.f,0.f,50.0f);

    // Calculate the straight-line direction to the target
    Direction = (TargetLocation - _StartLocation).GetSafeNormal();

    // Calculate the distance to the target
    float DistanceToTarget = FVector::Dist(_StartLocation, Target);

    // Calculate the time it takes to reach the target based on speed
    TimeToTarget = DistanceToTarget / Speed;

    // Initialize elapsed time
    ElapsedTime = 0.0f;
    StartLocation = _StartLocation;
}

void AGenericProjectile::InitializeProjectileWithStraightPath(const FVector& _StartLocation, const FVector& Target)
{
    SetActorLocation(_StartLocation);
    TargetLocation = Target;

    // Calculate the straight-line direction to the target
    Direction = (TargetLocation - _StartLocation).GetSafeNormal();

    // Calculate time needed to reach the target
    float DistanceToTarget = FVector::Dist(_StartLocation, TargetLocation);
    TimeToTarget = DistanceToTarget / Speed;

    // Initialize elapsed time
    ElapsedTime = 0.0f;
    StartLocation = _StartLocation;
}

void AGenericProjectile::InitializeProjectile(const FVector& ShootDirection)
{
    Direction = ShootDirection.GetSafeNormal();
}

void AGenericProjectile::SetProjectileUnitRefrence(TWeakObjectPtr<APlayerUnit> _unitref, TWeakObjectPtr<APlayerUnit> _enemy)
{
	UnitRef = _unitref;
	EnemyRef = _enemy;
	// Set the projectile's damage and unit reference
	UE_LOG(LogTemp, Warning, TEXT("Projectile initialized with damage: %d"), damage);
}

void AGenericProjectile::StraightShot()
{

}

void AGenericProjectile::DealDamage()
{
	if (CombatManager.IsValid() && UnitRef.IsValid() && EnemyRef.IsValid())
	{
		// Deal damage to the target unit
		CombatManager->DealDamageToUnit(UnitRef.Get(),EnemyRef.Get());
		//UE_LOG(LogTemp, Warning, TEXT("Dealing %d damage to %s"), damage, *UnitRef->GetName());
		UnitRef->FinishTurn();
		UnitRef->EnemyToAttack = nullptr; // Reset the target after dealing damage
        UnitRef = nullptr;
		EnemyRef = nullptr;

	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("CombatManager or UnitRef is invalid!"));
	}
    Destroy();
}

void AGenericProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor && OtherActor != this)
    {
        // Apply damage to the hit actor
        //UGameplayStatics::ApplyDamage(OtherActor, Damage, GetInstigatorController(), this, nullptr);
        UE_LOG(LogTemp, Warning, TEXT("Hit Target"));
        // Destroy the projectile
		
        Destroy();
    }
}

// Called when the game starts or when spawned
void AGenericProjectile::BeginPlay()
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

}

// Called every frame
void AGenericProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    // Move the projectile
    if (ElapsedTime < TimeToTarget && !bUseStraightPath)
    {
        ElapsedTime += DeltaTime;
        FVector PreviousPosition = GetActorLocation();
        // Calculate the linear interpolation for movement
        float Alpha = ElapsedTime / TimeToTarget;

        // Interpolate between the start and target location
        FVector LinearPosition = FMath::Lerp(StartLocation, TargetLocation, Alpha);

        // Create the arched motion: Use a parabola for the Z-axis
        float Midpoint = 0.5f; // The normalized point where the apex is reached (at 50% of the flight time)
        float ApexHeight = 150.0f; // Height of the apex (adjust as needed)

        // Calculate parabolic offset (using a quadratic curve)
        float ParabolicOffset = (-4.0f * ApexHeight * FMath::Pow(Alpha - Midpoint, 2)) + ApexHeight;

        // Add the parabolic offset to the Z-axis
        FVector CurvedPosition = LinearPosition + FVector(0.0f, 0.0f, ParabolicOffset);

        // Calculate the direction of movement (towards the next point)
        FVector CurrentDirection = (CurvedPosition - GetActorLocation()).GetSafeNormal();

        // Move the projectile to the new position
        SetActorLocation(CurvedPosition);

        // Smoothly interpolate the rotation to match the direction
        FVector MovementVector = (CurvedPosition - PreviousPosition).GetSafeNormal();

        // Set the rotation to align with the movement vector
        if (!MovementVector.IsNearlyZero())
        {
            FRotator TargetRotation = FRotationMatrix::MakeFromX(MovementVector).Rotator();
           // TargetRotation.Yaw += 180.0f;
            SetActorRotation(TargetRotation); // Directly set the rotation
        }


        // Destroy the projectile if it reaches the target
        if (Alpha >= 1.0f)
        {
            DealDamage();
            
           
        }
    }
    if (bUseStraightPath) // Add a boolean flag to choose the path type
    {
        if (ElapsedTime < TimeToTarget)
        {
            ElapsedTime += DeltaTime;
            FVector PreviousPosition = GetActorLocation();
            // Calculate the linear interpolation for movement
            float Alpha = ElapsedTime / TimeToTarget;

            // Interpolate between the start and target location
            FVector LinearPosition = FMath::Lerp(StartLocation, TargetLocation, Alpha);

            // Create the arched motion: Use a parabola for the Z-axis
            float Midpoint = 0.5f; // The normalized point where the apex is reached (at 50% of the flight time)
            float ApexHeight = 150.0f; // Height of the apex (adjust as needed)

            // Calculate parabolic offset (using a quadratic curve)
            float ParabolicOffset = (-4.0f * ApexHeight * FMath::Pow(Alpha - Midpoint, 2)) + ApexHeight;

            // Add the parabolic offset to the Z-axis
            FVector CurvedPosition = LinearPosition + FVector(0.0f, 0.0f, ParabolicOffset);

            // Calculate the direction of movement (towards the next point)
            FVector CurrentDirection = (CurvedPosition - GetActorLocation()).GetSafeNormal();
            
            // Move in a straight line
            FVector NewLocation = StartLocation + (Direction * Speed * ElapsedTime);
            SetActorLocation(NewLocation);

            FVector MovementVector = (CurvedPosition - PreviousPosition).GetSafeNormal();

            // Set the rotation to align with the movement vector
            if (!MovementVector.IsNearlyZero())
            {
                FRotator TargetRotation = FRotationMatrix::MakeFromX(MovementVector).Rotator();
                // TargetRotation.Yaw += 180.0f;
                SetActorRotation(TargetRotation); // Directly set the rotation
            }
            // Destroy projectile when reaching target
            if (ElapsedTime >= TimeToTarget)
            {
				DealDamage();
            }
        }
    }
}

