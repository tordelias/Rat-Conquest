// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCamera.h"

//Engine
#include "Camera/CameraComponent.h"
#include "MyPlayerController.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"


//includes
#include "Rat_Conquest/Components/InteractionInterface.h"

// Sets default values
APlayerCamera::APlayerCamera()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(25.f, 50.0f);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArm->SetupAttachment(GetCapsuleComponent());
	SpringArm->bUsePawnControlRotation = true;


	ThirdPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCameraComponent->SetupAttachment(SpringArm);
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Overlap);


    MinZoom = 100.0f;
    MaxZoom = 1000.0f;
    ZoomSpeed = 30.0f;

}

// Called when the game starts or when spawned
void APlayerCamera::BeginPlay()
{
    Super::BeginPlay();
    InteractionCheckDistance = 3000.f;

}

// Called every frame
void APlayerCamera::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (GetWorld()->TimeSince(InteractionData.LastInteractionCheckTime) > InteractionCheckFrequency)
    {
        PerformInteractionCheck();
    }

}

void APlayerCamera::PerformInteractionCheck()
{
    // Get mouse world position and direction
    FVector MouseWorldLocation, MouseWorldDirection;
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        PC->DeprojectMousePositionToWorld(MouseWorldLocation, MouseWorldDirection);
    }
    else
    {
        NoInteractableFound();
        return;
    }

    FVector TraceEnd = MouseWorldLocation + (MouseWorldDirection * InteractionCheckDistance);

    // Perform a line trace from the mouse location into the world
    FHitResult MouseHit;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);

    bool bHitSomething = GetWorld()->LineTraceSingleByChannel(MouseHit, MouseWorldLocation, TraceEnd, ECC_Visibility, QueryParams);

    AActor* HitActor = nullptr;
    if (bHitSomething)
    {
        HitActor = MouseHit.GetActor();
    }

    if (HitActor && HitActor->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
    {
        if (HitActor != InteractionData.CurrentInteractable)
        {
            FoundInteractable(HitActor);
            return;
        }

    }
    NoInteractableFound();
}

void APlayerCamera::FoundInteractable(AActor* newInteractable)
{
    if (bIsInteracting())
    {
        EndInteract();
    }
    if (InteractionData.CurrentInteractable)
    {
        TargetInteractable = InteractionData.CurrentInteractable;
        TargetInteractable->EndFocus();
    }

    InteractionData.CurrentInteractable = newInteractable;
    TargetInteractable = newInteractable;

	//Show & Update Widget

    TargetInteractable->BeginFocus();
}

void APlayerCamera::NoInteractableFound()
{
    if (bIsInteracting())
    {
        GetWorldTimerManager().ClearTimer(TimerHandleInteraction);
    }
    if (InteractionData.CurrentInteractable)
    {
        if (IsValid(TargetInteractable.GetObject()))
        {
            TargetInteractable->EndFocus();
        }

        //Close Widget

        InteractionData.CurrentInteractable = nullptr;
        TargetInteractable = nullptr;
    }
}

void APlayerCamera::BeginInteract()
{
	PerformInteractionCheck();
    if (InteractionData.CurrentInteractable)
    {
        if (IsValid(TargetInteractable.GetObject()))
        {
            TargetInteractable->BeginInteract();
            if (FMath::IsNearlyZero(TargetInteractable->InteractableData.InteractionDuration, 0.1f))
            {
                Interact();
            }
            else
            {
                GetWorldTimerManager().SetTimer(TimerHandleInteraction, this, &APlayerCamera::Interact, TargetInteractable->InteractableData.InteractionDuration, false);
            }
        }
    }
}

void APlayerCamera::EndInteract()
{
    GetWorldTimerManager().ClearTimer(TimerHandleInteraction);

    if (IsValid(TargetInteractable.GetObject()))
    {
        TargetInteractable->EndInteract();
    }
}

void APlayerCamera::Interact()
{
   if( IsValid(TargetInteractable.GetObject()))
   {
	   TargetInteractable->Interact(this);
   }
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

    // Adjust the speed of the zoom
    float NewLength = SpringArm->TargetArmLength + (ZoomAxis * ZoomSpeed);

    // Clamp the arm length to the specified range
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

