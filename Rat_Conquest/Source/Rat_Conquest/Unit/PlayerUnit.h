// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Rat_Conquest/Components/InteractionInterface.h"
#include "PlayerUnit.generated.h"

class UHealthBar;
class UWidgetComponent;
class AGridManager;
class AGameManager;
class ACombatManager;
class AGridTile;
class AItem;
class AGenericProjectile;
class MutationData;
class AWeapon;

DECLARE_DELEGATE(FOnMovementCompleteSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHealthChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBlendSpaceChange);


UCLASS()
class RAT_CONQUEST_API APlayerUnit : public APawn, public IInteractionInterface
{
    GENERATED_BODY()

public:
    // Constructor
    APlayerUnit();

    // Mesh & Visuals
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
    TObjectPtr<USkeletalMeshComponent> SkeletalMesh;

    UPROPERTY(EditAnywhere, Category = "Mesh")
    TObjectPtr<UStaticMeshComponent> WeaponMesh;

    UPROPERTY(EditAnywhere, Category = "Mesh")
    TObjectPtr<UStaticMeshComponent> ArmorMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Icon")
    TObjectPtr<UTexture2D> UnitIcon;

    // Grid Variables
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
    FVector2D CurrentGridPosition;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
    FVector2D GridStartPosition;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
    TWeakObjectPtr<AGridManager> GridManager;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
    TWeakObjectPtr<AGameManager> GameManager;

    // Combat Attributes
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Manager")
    TWeakObjectPtr<ACombatManager> CombatManager;

    UPROPERTY(EditAnywhere)
    TObjectPtr<UWidgetComponent> HealthBarWidgetComponent;

    UPROPERTY()
    TWeakObjectPtr<AItem> Weapon;

    UPROPERTY()
    TWeakObjectPtr<AItem> Armour;

    UPROPERTY()
    TWeakObjectPtr<AItem> Artifact;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
    int MovementSpeed = 3;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
    int Health = 3;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
    int maxHealth = 5;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
    int Damage = 1;

    int MinDamage = 1;
    int MaxDamage = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
    int Defence = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
    int AttackRange = 6;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
    int Initiative = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
    int Attack = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
    int experienceReward = 10;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
    bool bIsPlayerUnit = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
    bool bIsRangedUnit = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
    FName UnitName;

    // Inventory & Equipment
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
    TSubclassOf<AItem> StartWeapon;

    // Animation
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    FVector2D animationToPlay; // 0 = idle, 25 = walk, 50 = attack, 75 = secondary attack, 100 = death

    // Projectile
    UPROPERTY(EditAnywhere, Category = "Projectile")
    TSubclassOf<AGenericProjectile> ProjectileClass;

    // Interaction
    UPROPERTY(VisibleInstanceOnly, Category = "Interact")
    FInteractableData InstanceInteractableData;

    // Delegates
    FOnMovementCompleteSignature OnMovementComplete;

    UPROPERTY(BlueprintAssignable, Category = "Health")
    FOnHealthChanged OnHealthChanged;

    UPROPERTY(BlueprintAssignable, Category = "Animation")
    FOnBlendSpaceChange OnBlendSpaceChange;

    UFUNCTION(BlueprintCallable, Category = "Animation")
    void NotifyBlendSpaceChange();

    //current blend space
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlendSpace")
    TObjectPtr<UAnimBlueprint> CurrentAnimInstance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlendSpace")
    TObjectPtr<UAnimBlueprint> AnimBlueprint;

    //Sound & Effects
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound & Effects")
    TObjectPtr<USoundBase> SB_Walk;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound & Effects")
    TObjectPtr<USoundBase> SB_Attack;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound & Effects")
    TObjectPtr<USoundBase> SB_PickupItem;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound & Effects")
    TObjectPtr<USoundBase> SB_Mutate;

    // Public Functions
    void MoveToTile(FVector2D NewGridPosition);
    bool CalculatePathToTile(FVector2D InTargetGridPosition);
    TArray<FVector2D> GetPathToTile(FVector2D InTargetGridPosition, FVector2D StartTile);
    void SetInitalPosition(FVector2D position);
    void DelayedInitalPosition();
    void ResetPosition();
    float ChebyshevDistance(FVector2D A, FVector2D B);

    void PlayerAttack(TWeakObjectPtr<APlayerCamera> PlayerCharacter);
    void AttackAfterMovement();
    void ShootProjectile(FVector _EnemyLocation);
    void KillAfterAnim();
    void DestoryUnit();
    void ExecutePlayerTurn();
    void ExecuteAITurn();
    void FinishTurn();

    void CheckForGridObjects();
    void CheckForItems();
    void CalculateStats();
    void DropItem(TWeakObjectPtr<AItem> OldItem, FVector2D CurrentPosition);
    void EquipStartWeapon(TWeakObjectPtr<AItem> ItemToAdd);
    void UseCurrentItem();

    void Mutate();
    TArray<int> GetMutationC1();
    TArray<int> GetMutationC2();
    TArray<int> GetMutationC3();
    float GetMouseRotationToEnemy(TWeakObjectPtr<APlayerCamera> Camera);
    void ApplyMutation(TArray<int>);

    void BeginFocus();
    void EndFocus();
    virtual void BeginMouseHoverFocus() override;
    virtual void EndMouseHoverFocus() override;
    virtual void Interact(TWeakObjectPtr<APlayerCamera> PlayerCharacter) override;

    virtual void Tick(float DeltaTime) override;
    virtual void BeginPlay() override;
    void UpdateInteractableData();

    void UpdateHealthBar();
    void UpdateHealthBarRotation();

	void SetTurnAngle(float Angle) { TurnAngle = Angle; }

    virtual void RandomizeStats();

    FVector2D GetCardinalDirection(FVector2D FromGridPos, FVector2D ToGridPos);
    FVector2D GetMousePosition(FVector WorldLocation, FVector WorldDirection);

    // Getters
    TObjectPtr<USkeletalMeshComponent> GetMesh() { return SkeletalMesh; }

    // Public Variables
    UPROPERTY()
    bool bHasFinishedTurn = false;

    bool bFirstMove = true;
    bool bHasWeapon = false;
    bool bIsCurrentUnit = false;

    TWeakObjectPtr<APlayerUnit> EnemyToAttack;
    FVector2D TargetEnemyLocation = FVector2D(0, 0);
    FVector2D MouseGridPos = FVector2D(0, 0);

    TArray<TWeakObjectPtr<AItem>> ItemSlots; // 0 = weapon, 1 = armor, 2 = accessory
    std::shared_ptr<MutationData> mutationData;

    FTimerHandle DeathTimer;

    // Knockback Handling
    bool bIsKnockbackActive = false;
    float KnockbackProgress = 0.0f;
    float KnockbackDuration = 1.f;
    FVector KnockbackStartPosition;
    FVector KnockbackTargetPosition;

private:

	void ResetRotation();
    // Private Variables
    int unitLevel = 0;
    bool bIsMoving = false;

    float TurnAngle = 0.0f; 

    float MovementProgress = 0.0f;
    float MovementDuration = 0.5f;
    FVector StartPosition;
    FVector TargetPosition;
    FVector2D TargetGridPosition;
    TArray<FVector2D> PathToTake;

    // Tiles
    TArray<TWeakObjectPtr<AGridTile>> MovedTiles;
    TArray<TWeakObjectPtr<AGridTile>> HoverTiles;

    UPROPERTY()
    TWeakObjectPtr<UHealthBar> HealthBarWidget;
};