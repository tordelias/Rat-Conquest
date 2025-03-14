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
#include "Rat_Conquest/GridTile/GridTile.h"

// Sets default values
APlayerCamera::APlayerCamera()
{
    PrimaryActorTick.bCanEverTick = true;

    GetCapsuleComponent()->InitCapsuleSize(25.f, 50.0f);

    SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
    SpringArm->SetupAttachment(GetCapsuleComponent());

    // Fix: Disable Pawn Control Rotation
    SpringArm->bUsePawnControlRotation = false;
    SpringArm->bInheritPitch = false;
    SpringArm->bInheritYaw = false;
    SpringArm->bInheritRoll = false;

    ThirdPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
    ThirdPersonCameraComponent->SetupAttachment(SpringArm);

    bUseControllerRotationYaw = false;

    GetCharacterMovement()->bOrientRotationToMovement = false;
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Overlap);

    MinZoom = 500.0f;
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

    // Set Camera to Top-Down View
    SetCameraTopDown(0.0f, 1000.0f);

    if (SpringArm)
    {
        SpringArm->bInheritYaw = true;  // Allow mouse rotation
    }
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
        //cast to gridTile
		AGridTile* GridTile = Cast<AGridTile>(TargetInteractable.GetObject());

        if (PlayerUnit)
        {
            if (mainHUD)
            {
                mainHUD->ShowStatWidget();
				mainHUD->UpdateStatWidget(&PlayerUnit->InstanceInteractableData);
            }
            if (!PlayerUnit->bIsPlayerUnit)
            {
                UE_LOG(LogTemp, Error, TEXT("Successfully found Enemy"));
				SwitchMouseCursor(PlayerUnit);
            }
        }
		else if (GridTile)
        {
            if (GridTile->unitRefrence)
            {
	            if (!GridTile->unitRefrence->bIsPlayerUnit)
				{
					SwitchMouseCursor(GridTile->unitRefrence);
				}
                else
				{
					if (AMyPlayerController* PC = Cast<AMyPlayerController>(GetController()))
					{
						PC->UseMouseDefaultPointer();
					}
				}
            }
            else
            {
                if (AMyPlayerController* PC = Cast<AMyPlayerController>(GetController()))
                {
                    PC->UseMouseDefaultPointer();
                }
            }
        }
        else
        {
            if (AMyPlayerController* PC = Cast<AMyPlayerController>(GetController()))
            {
                PC->UseMouseDefaultPointer();
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

            // Interaction logic
            if (FMath::IsNearlyZero(TargetInteractable->InteractableData.InteractionDuration, 0.1f))
            {
                Interact();
            }
            else
            {
                GetWorldTimerManager().SetTimer(
                    TimerHandleInteraction,
                    this,
                    &APlayerCamera::Interact,
                    TargetInteractable->InteractableData.InteractionDuration,
                    false
                );
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

void APlayerCamera::SwitchMouseCursor(TObjectPtr<APlayerUnit> Enemy)
{
    if (AMyPlayerController* PC = Cast<AMyPlayerController>(GetController())) // Cast to your custom controller
    {
        if (Enemy && CurrentUnit)
        {
            float Range = CurrentUnit->MovementSpeed;
            FVector2D PlayerLocation = CurrentUnit->CurrentGridPosition;
            float Distance = FVector2D::Distance(PlayerLocation, Enemy->CurrentGridPosition);

            if (Distance <= Range + 1)
            {
                // Use melee attack pointer when in range
                PC->UseMouseMeleeAttackPointer();

                // Get the mouse rotation to the enemy
                float RotationAngle = Enemy->GetMouseRotationToEnemy(this);

                // Adjust the rotation angle based on the camera's yaw
                // Get the camera's yaw (only horizontal rotation)
                FRotator CameraRotation = ThirdPersonCameraComponent->GetComponentRotation(); // Get the camera's rotation
                
                float CameraYaw = CameraRotation.Yaw;

                // Adjust the mouse rotation by subtracting the camera's yaw to make it relative to the camera
                float AdjustedRotation = RotationAngle - CameraYaw;

                // Normalize the angle to ensure it's within a valid range
                if (AdjustedRotation < 0.0f)
                {
                    AdjustedRotation += 360.0f;
                }
                else if (AdjustedRotation >= 360.0f)
                {
                    AdjustedRotation -= 360.0f;
                }

                // Apply the adjusted rotation to the mouse pointer
                PC->changeMouseRotation(AdjustedRotation);
            }
            else
            {
                // Default mouse pointer when out of range
                PC->UseMouseDefaultPointer();
            }
        }
    }
}



void APlayerCamera::Look(const FInputActionValue& Value)
{
    FVector2D LookAxis = Value.Get<FVector2D>();

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (PC->IsInputKeyDown(EKeys::RightMouseButton)) // Rotate only when RMB is held
        {
            if (SpringArm)
            {
                // Get current rotation
                FRotator CurrentRotation = SpringArm->GetRelativeRotation();

                // Apply yaw rotation freely (left/right movement)
                SpringArm->AddLocalRotation(FRotator(0.0f, -LookAxis.X, 0.0f));

                // Apply pitch rotation, but clamp it to avoid flipping
                float NewPitch = FMath::Clamp(CurrentRotation.Pitch + -LookAxis.Y, -89.0f, -30.0f);
                SpringArm->SetRelativeRotation(FRotator(NewPitch, SpringArm->GetRelativeRotation().Yaw, 0.0f));
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
void APlayerCamera::SetCameraTopDown(float ZRotation, float Height)
{
    if (SpringArm)
    {
        // Fix: Use SetWorldRotation to ensure the camera rotates
        FRotator NewRotation = FRotator(-60.0f, ZRotation, 0.0f);
        SpringArm->SetWorldRotation(NewRotation);

        // Adjust height correctly
        SpringArm->TargetArmLength = FMath::Clamp(Height, MinZoom, MaxZoom);
    }
}

