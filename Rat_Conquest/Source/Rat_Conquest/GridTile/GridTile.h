// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Rat_Conquest/Components/InteractionInterface.h"
#include "GridTile.generated.h"

class APlayerUnit;
class AItem;
class AInteractableGridObject;
class UStaticMeshComponent;


UCLASS()
class RAT_CONQUEST_API AGridTile : public AActor, public IInteractionInterface
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AGridTile();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UStaticMeshComponent> TileMesh;

    bool bIsOccupied = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
    FVector2D GridPosition;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
    bool bIsBridgeTile = false;

    void BeginFocus();
    void EndFocus();
    void GreenHighlight();
    void RedHighlight();
    void YellowHighlight();
    void EndHighlight();
    virtual void BeginMouseHoverFocus() override;
    virtual void EndMouseHoverFocus() override;
    virtual void Interact(TWeakObjectPtr<APlayerCamera> PlayerCharacter) override;

    UPROPERTY(VisibleInstanceOnly, Category = "Pickup")
    FInteractableData InstanceInteractableData;

    UPROPERTY()
    TWeakObjectPtr<APlayerUnit> unitRefrence;

    void UpdateInteractableData();
    void SetUnitReference(TWeakObjectPtr<APlayerUnit> Unit);
    void RemoveUnitReference();

    UPROPERTY(VisibleAnywhere, Category = "Occupants")
    TArray<TWeakObjectPtr<AActor>> TileObjects;

    UPROPERTY()
    TWeakObjectPtr<AItem> ItemSlot;

    UPROPERTY()
	TWeakObjectPtr<AInteractableGridObject> InteractableObjectSlot;

    void AddOccupant(TWeakObjectPtr<AActor> TileObj);
    void RemoveOccupant(TWeakObjectPtr<AActor> TileObj);
    bool IsTileOccupied();
    bool CheckIfTileOccupied();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Flags for highlighting
    bool bIsHighlightedByUnit = false;
    bool bIsHighlightedByGameManager = false;
    bool bIsHovered = false;

    bool bIsGreenHighlighted = false;
    bool bIsRedHighlighted = false;

    // For A* pathfinding
    float G = 0.f;
    float H = 0.f;
    float F = 0.f;
    void ResetPathfindingData();
};