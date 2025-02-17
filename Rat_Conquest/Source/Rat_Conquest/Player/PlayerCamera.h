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
	AActor* CurrentInteractable;

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
	UCameraComponent* ThirdPersonCameraComponent;

	// Spring arm component to position the camera behind the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArm;


	// Zoom range for the camera
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float MinZoom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float MaxZoom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float ZoomSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Look;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Zoom;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Interact;

private: 
	void Look(const FInputActionValue& Value);

	// Zoom function to zoom the camera in/out
	void Zoom(const FInputActionValue& Value);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Interaction
	UPROPERTY(VisibleInstanceOnly, Category = "Interaction")
	TScriptInterface<IInteractionInterface> TargetInteractable;

	float InteractionCheckFrequency;

	float InteractionCheckDistance;

	FTimerHandle TimerHandleInteraction;

	FInteractionData InteractionData;
	
	void PerformInteractionCheck();

	void FoundInteractable(AActor* newInteractable);

	void NoInteractableFound();

	void BeginInteract();

	void EndInteract();

	void Interact();

	//PlayerUnit movement
	APlayerUnit* CurrentUnit;

	AMainHUD* mainHUD; 

	FVector MouseWorldLocation, MouseWorldDirection;



public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SetCurrentUnit(APlayerUnit* NewUnit) { CurrentUnit = NewUnit; };

	APlayerUnit* GetCurrentUnit() const { return CurrentUnit; }

	FORCEINLINE bool bIsInteracting() const { return GetWorldTimerManager().IsTimerActive(TimerHandleInteraction); }

	FVector GetMouseWorldLocation() const { return MouseWorldLocation; }
	FVector GetMouseWorldDirection() const { return MouseWorldDirection; }

};
