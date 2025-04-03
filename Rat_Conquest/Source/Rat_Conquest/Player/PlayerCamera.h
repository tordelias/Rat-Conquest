// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCamera.generated.h"

struct FInputActionValue;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class IInteractionInterface;
class APlayerUnit;
class AMainHUD;

USTRUCT()
struct FInteractionData
{
    GENERATED_USTRUCT_BODY()

    FInteractionData() : CurrentInteractable(nullptr), LastInteractionCheckTime(0.0f)
    {
    };

    UPROPERTY()
    TWeakObjectPtr<AActor> CurrentInteractable;

    UPROPERTY()
    float LastInteractionCheckTime;
};

UCLASS()
class RAT_CONQUEST_API APlayerCamera : public ACharacter
{
    GENERATED_BODY()

public:
    // Sets default values for this character's properties
    APlayerCamera();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    TObjectPtr<UCameraComponent> ThirdPersonCameraComponent;

    // Spring arm component to position the camera behind the character
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    TObjectPtr<USpringArmComponent> SpringArm;

    // Zoom range for the camera
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float MinZoom;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float MaxZoom;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float ZoomSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> IA_Look;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> IA_Zoom;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> IA_Interact;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> IA_Move;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> IA_MMB;

private:

    void Look(const FInputActionValue& Value);
    void Zoom(const FInputActionValue& Value);
	void MoveCamera(const FInputActionValue& Value);
	void MMBPressed(const FInputActionValue& Value);
	void MMBReleased(const FInputActionValue& Value);

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    // Interaction
    UPROPERTY(VisibleInstanceOnly, Category = "Interaction")
    TScriptInterface<IInteractionInterface> TargetInteractable;

    float InteractionCheckFrequency;
    float InteractionCheckDistance;
    FTimerHandle TimerHandleInteraction;
    FInteractionData InteractionData;

    void PerformInteractionCheck();
    void FoundInteractable(TWeakObjectPtr<AActor> NewInteractable);
    void NoInteractableFound();
    void BeginInteract();
    void EndInteract();
    void Interact();
    void SwitchMouseCursor(TWeakObjectPtr<APlayerUnit> Enemy);

	void SetCameraPosition(FVector NewPosition);
	FVector NewCameraPosition;
	FVector OldCameraPosition;

    bool bIsDeaultCursor = true;
    bool bIsMiddleMouseDown = false;

    // PlayerUnit movement
    UPROPERTY()
    TWeakObjectPtr<APlayerUnit> CurrentUnit;

    UPROPERTY()
    TWeakObjectPtr<AMainHUD> mainHUD;

    FVector MouseWorldLocation, MouseWorldDirection;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    void SetCurrentUnit(TWeakObjectPtr<APlayerUnit> NewUnit) { CurrentUnit = NewUnit; };
    TWeakObjectPtr<APlayerUnit> GetCurrentUnit() const { return CurrentUnit; }

    void SetCameraTopDown(float ZRotation, float height);

    FORCEINLINE bool bIsInteracting() const { return GetWorldTimerManager().IsTimerActive(TimerHandleInteraction); }

    FVector GetMouseWorldLocation() const { return MouseWorldLocation; }
    FVector GetMouseWorldDirection() const { return MouseWorldDirection; }
};