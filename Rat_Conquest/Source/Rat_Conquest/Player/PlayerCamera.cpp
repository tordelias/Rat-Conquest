// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCamera.h"

// Engine
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

// Includes
#include "Rat_Conquest/Components/InteractionInterface.h"
#include "Rat_Conquest/Unit/PlayerUnit.h"
#include "Rat_Conquest/Widgets/MainHUD.h"

// Sets default values
APlayerCamera::APlayerCamera()
{
    // Set this character to call Tick() every frame. You can turn this off to improve performance if you don't need it.
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
    MaxZoom = 1200.0f;
    ZoomSpeed = 30.0f;
}

// Called when the game starts or when spawned
void APlayerCamera::BeginPlay()
{
    Super::BeginPlay();

	mainHUD = Cast<AMainHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());

    InteractionCheckDistance = 1750.f; 
    InteractionCheckFrequency = 0.05f;

   
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
        if (InteractionData.CurrentInteractable && HitActor == InteractionData.CurrentInteractable)
        {
            InteractionData.LastInteractionCheckTime = GetWorld()->GetTimeSeconds();
            return;
        }


        if (GetWorld()->GetTimeSeconds() - InteractionData.LastInteractionCheckTime > InteractionCheckFrequency) 
        {
            FoundInteractable(HitActor);
        }
    }
    else
    {

        if (GetWorld()->GetTimeSeconds() - InteractionData.LastInteractionCheckTime > InteractionCheckFrequency)
        {
            NoInteractableFound();
        }
    }
}

void APlayerCamera::FoundInteractable(AActor* NewInteractable)
{
    if (InteractionData.CurrentInteractable == NewInteractable)
    {
        return;
    }

    if (bIsInteracting())
    {
        EndInteract();
    }

    if (InteractionData.CurrentInteractable)
    {
        TargetInteractable = InteractionData.CurrentInteractable;
        TargetInteractable->EndMouseHoverFocus();
    }

    InteractionData.CurrentInteractable = NewInteractable;
    TargetInteractable = NewInteractable;
    TargetInteractable->BeginMouseHoverFocus();
    InteractionData.LastInteractionCheckTime = GetWorld()->GetTimeSeconds();

    if (TargetInteractable)
    {
        APlayerUnit* PlayerUnit = Cast<APlayerUnit>(TargetInteractable.GetObject());

        if (PlayerUnit)
        {
            //UE_LOG(LogTemp, Error, TEXT("Successfully casted to APlayerUnit"));
            if (mainHUD)
            {
                mainHUD->ShowStatWidget();
				mainHUD->UpdateStatWidget(&PlayerUnit->InstanceInteractableData);
            }
        }
    }
}

void APlayerCamera::NoInteractableFound()
{
    if (!InteractionData.CurrentInteractable)
    {
        return;
    }

    if (bIsInteracting())
    {
        GetWorldTimerManager().ClearTimer(TimerHandleInteraction);
    }

    if (InteractionData.CurrentInteractable)
    {
        if (IsValid(TargetInteractable.GetObject()))
        {
            TargetInteractable->EndMouseHoverFocus();
        }

        // Close widget

        if (mainHUD)
        {
            mainHUD->CloseStatWidget();
        }
        InteractionData.CurrentInteractable = nullptr;
        TargetInteractable = nullptr;
    }

    InteractionData.LastInteractionCheckTime = GetWorld()->GetTimeSeconds();
}


void APlayerCamera::BeginInteract()
{
    PerformInteractionCheck();
    if (InteractionData.CurrentInteractable)
    {
        if (IsValid(TargetInteractable.GetObject()))
        {
            TargetInteractable->BeginMouseHoverFocus();
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
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (!PC->IsInputKeyDown(EKeys::RightMouseButton))
        {
            if (IsValid(TargetInteractable.GetObject()))
            {
                TargetInteractable->Interact(this);
            }
        }
    }
}

void APlayerCamera::Look(const FInputActionValue& Value)
{
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (PC->IsInputKeyDown(EKeys::RightMouseButton))
        {
            FVector2D LookAxis = Value.Get<FVector2D>();
            if (Controller != nullptr)
            {
                AddControllerYawInput(-LookAxis.X);
                AddControllerPitchInput(LookAxis.Y);
            }
        }
    }
}

void APlayerCamera::Zoom(const FInputActionValue& Value)
{
    float ZoomAxis = Value.Get<float>();

    float NewLength = SpringArm->TargetArmLength + (ZoomAxis * ZoomSpeed);

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
        EnhancedInputComponent->BindAction(IA_Interact, ETriggerEvent::Started, this, &APlayerCamera::BeginInteract);
    }
}
