// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCamera.h"

//Engine
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/LocalPlayer.h"

// Sets default values
APlayerCamera::APlayerCamera()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // Create and set up the camera boom
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->TargetArmLength = 300.0f; // Default zoom
    SpringArm->bUsePawnControlRotation = false;

    // Create and set up the third-person camera
    ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
    ThirdPersonCamera->SetupAttachment(SpringArm);

    // Initialize default values
    CameraRotationRate = 100.0f;
    MinZoom = 200.0f;
    MaxZoom = 500.0f;

}

// Called when the game starts or when spawned
void APlayerCamera::BeginPlay()
{
    Super::BeginPlay();

}

// Called every frame
void APlayerCamera::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

void APlayerCamera::Look(const FInputActionValue& Value)
{
	FVector2D LookAxis = Value.Get<FVector2D>();
    if (Controller != nullptr)
    {
        // add yaw and pitch input to controller
        AddControllerYawInput(-LookAxis.X);
        AddControllerPitchInput(LookAxis.Y);
    }
}

void APlayerCamera::Zoom(const FInputActionValue& Value)
{
	float ZoomAxis = Value.Get<float>();
	float NewLength = SpringArm->TargetArmLength + ZoomAxis * 100.0f;
	SpringArm->TargetArmLength = FMath::Clamp(NewLength, MinZoom, MaxZoom);
}



// Called to bind functionality to input
void APlayerCamera::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputComponent->BindAction(IA_Look, ETriggerEvent::Triggered, this, &APlayerCamera::Look);

		EnhancedInputComponent->BindAction(IA_Zoom, ETriggerEvent::Triggered, this, &APlayerCamera::Zoom);

    }
}

