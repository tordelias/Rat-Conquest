// Fill out your copyright notice in the Description page of Project Settings.

// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCamera.h"
#include "Camera/CameraComponent.h"
#include "MyPlayerController.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Rat_Conquest/Components/InteractionInterface.h"
#include "Rat_Conquest/Unit/PlayerUnit.h"
#include "Rat_Conquest/Widgets/MainHUD.h"
#include "Rat_Conquest/GridTile/GridTile.h"
#include "Rat_Conquest/Items/ItemBase.h"

#include "Camera/CameraComponent.h" // For UCameraComponent
#include "GameFramework/SpringArmComponent.h" // For USpringArmComponent

APlayerCamera::APlayerCamera()
{
    PrimaryActorTick.bCanEverTick = true;

    // Disable capsule collision (since this is camera-only)
    GetCapsuleComponent()->InitCapsuleSize(25.f, 50.0f);
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // SpringArm setup
    SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
    SpringArm->SetupAttachment(GetCapsuleComponent());

    // Rotation settings
    SpringArm->bUsePawnControlRotation = false;
    SpringArm->bInheritPitch = false;
    SpringArm->bInheritYaw = false;
    SpringArm->bInheritRoll = false;

    // Collision configuration
    SpringArm->bDoCollisionTest = true;
    SpringArm->ProbeSize = 12.0f; // Note: Correct spelling is ProbeSize (not ProbeSize)
    SpringArm->ProbeChannel = ECC_Visibility; // Use Visibility channel which typically works well for cameras

    // Prevent camera from getting too close
    SpringArm->CameraLagSpeed = 0.f; // Disable if not using camera lag
    SpringArm->bEnableCameraRotationLag = false;
    SpringArm->bEnableCameraLag = false;
    SpringArm->TargetArmLength = MaxZoom; // Start at max zoom
    SpringArm->SocketOffset = FVector(0, 0, 60.f); 

    // Camera setup
    ThirdPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
    ThirdPersonCameraComponent->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
    ThirdPersonCameraComponent->SetRelativeRotation(FRotator(0, 0, 0));
   // ThirdPersonCameraComponent->bUsePawnViewRotation = false;

    // Movement settings
    bUseControllerRotationYaw = false;
    if (GetCharacterMovement())
    {
        GetCharacterMovement()->bOrientRotationToMovement = false;
    }

    // Zoom settings
    MinZoom = 500.0f;
    MaxZoom = 1200.0f;
    ZoomSpeed = 30.0f;
}


void APlayerCamera::BeginPlay()
{
    Super::BeginPlay();

    mainHUD = Cast<AMainHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
    InteractionCheckDistance = 2050.f;
    InteractionCheckFrequency = 0.05f;
    SetCameraTopDown(0.0f, 1000.0f);

    if (IsValid(SpringArm.Get()))
    {
        SpringArm->bInheritYaw = true;
    }
}

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
    AActor* HitActor = bHitSomething ? MouseHit.GetActor() : nullptr;

    if (HitActor && HitActor->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
    {
        if (InteractionData.CurrentInteractable.IsValid() && HitActor == InteractionData.CurrentInteractable)
        {
            if (AGridTile* GridTile = Cast<AGridTile>(InteractionData.CurrentInteractable))
            {
                if (GridTile->unitRefrence.Get())
                {
                    if (!GridTile->unitRefrence->bIsPlayerUnit)
                    {
                        SwitchMouseCursor(GridTile->unitRefrence);
                    }
                    else if (AMyPlayerController* PC = Cast<AMyPlayerController>(GetController()))
                    {
                        PC->UseMouseDefaultPointer();
                    }
                }
            }
            InteractionData.LastInteractionCheckTime = GetWorld()->GetTimeSeconds();
            return;
        }

        if (GetWorld()->GetTimeSeconds() - InteractionData.LastInteractionCheckTime > InteractionCheckFrequency)
        {
            FoundInteractable(HitActor);
        }
    }
    else if (GetWorld()->GetTimeSeconds() - InteractionData.LastInteractionCheckTime > InteractionCheckFrequency)
    {
        NoInteractableFound();
    }
}

void APlayerCamera::FoundInteractable(TWeakObjectPtr<AActor> NewInteractable)
{
    // Level 1: Basic null check
    if (!IsValid(NewInteractable.Get()))
    {
        UE_LOG(LogTemp, Warning, TEXT("FoundInteractable: Invalid actor"));
        return;
    }

    // Level 2: Check if this is the same interactable
    if (InteractionData.CurrentInteractable.Get() == NewInteractable)
    {
        return;
    }

    // Level 3: Thread-safe world check
    UWorld* World = GetWorld();
    if (!IsValid(World))
    {
        UE_LOG(LogTemp, Warning, TEXT("FoundInteractable: Invalid world"));
        return;
    }

    // Clean up previous interaction
    if (bIsInteracting())
    {
        EndInteract();
    }

    // Handle previous interactable with full validation
    if (AActor* OldInteractable = InteractionData.CurrentInteractable.Get())
    {
        if (IsValid(OldInteractable)) // Double-check validity
        {
            if (auto OldInterface = TScriptInterface<IInteractionInterface>(OldInteractable))
            {
                OldInterface->EndMouseHoverFocus();
            }
        }
    }

    // Update current interactable
    InteractionData.CurrentInteractable = NewInteractable;
        TargetInteractable = NewInteractable.Get();

    // Begin new interaction with validation
    if (IsValid(NewInteractable.Get()))
    {
        if (auto Interface = TScriptInterface<IInteractionInterface>(NewInteractable.Get()))
        {
            Interface->BeginMouseHoverFocus();
        }
    }

    InteractionData.LastInteractionCheckTime = World->GetTimeSeconds();

    // Handle PlayerUnit case
    if (APlayerUnit* PlayerUnit = Cast<APlayerUnit>(NewInteractable))
    {
        if (IsValid(PlayerUnit) && IsValid(mainHUD.Get()) && !PlayerUnit->bIsPlayerUnit)
        {
            SwitchMouseCursor(PlayerUnit);
        }
    }
    // Handle GridTile case with extreme caution
    else if (AGridTile* GridTile = Cast<AGridTile>(NewInteractable))
    {
        if (IsValid(GridTile))
        {
            // Create local weak reference copy
            const TWeakObjectPtr<APlayerUnit> UnitRef = GridTile->unitRefrence;

            // Validate before any access
            if (APlayerUnit* Unit = UnitRef.Get())
            {
                if (IsValid(Unit)) // Double validation
                {
                    if (!Unit->bIsPlayerUnit)
                    {
                        SwitchMouseCursor(Unit);
                    }
                    else if (AMyPlayerController* PC = Cast<AMyPlayerController>(GetController()))
                    {
                        if (IsValid(PC)) PC->UseMouseDefaultPointer();
                    }
                }
            }
            else if (AMyPlayerController* PC = Cast<AMyPlayerController>(GetController()))
            {
                if (IsValid(PC)) PC->UseMouseDefaultPointer();
            }
        }
    }
    // Default case
    else if (AMyPlayerController* PC = Cast<AMyPlayerController>(GetController()))
    {
        if (IsValid(PC)) PC->UseMouseDefaultPointer();
    }
}

void APlayerCamera::NoInteractableFound()
{
    if (!InteractionData.CurrentInteractable.IsValid())
    {
        return;
    }

    if (bIsInteracting())
    {
        GetWorldTimerManager().ClearTimer(TimerHandleInteraction);
    }

    if (InteractionData.CurrentInteractable.IsValid())
    {
        if (TargetInteractable)
        {
            TargetInteractable->EndMouseHoverFocus();
        }

        // Close widget
        if (mainHUD.IsValid())
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

    if (InteractionData.CurrentInteractable.IsValid())
    {
        if (TargetInteractable)
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

    if (TargetInteractable)
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
            if (TargetInteractable)
            {
                TargetInteractable->Interact(this);
            }
        }
    }
}

void APlayerCamera::SwitchMouseCursor(TWeakObjectPtr<APlayerUnit> Enemy)
{
    if (AMyPlayerController* PC = Cast<AMyPlayerController>(GetController())) // Cast to your custom controller
    {
        if (Enemy.IsValid() && CurrentUnit.IsValid())
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
                FRotator CameraRotation = ThirdPersonCameraComponent->GetComponentRotation();
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

void APlayerCamera::SetCameraPosition(FVector NewPosition)
{
	//Lerp camera position to new position
	if (IsValid(SpringArm.Get()))
	{
		SpringArm->SetWorldLocation(FMath::Lerp(SpringArm->GetComponentLocation(), NewPosition, 0.1f));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SpringArm is not valid"));
	}
}

void APlayerCamera::Look(const FInputActionValue& Value)
{
    FVector2D LookAxis = Value.Get<FVector2D>();

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (PC->IsInputKeyDown(EKeys::RightMouseButton)) // Rotate only when RMB is held
        {
            if (IsValid(SpringArm.Get()))
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

void APlayerCamera::MoveCamera(const FInputActionValue& Value)
{
    if (!bIsMiddleMouseDown)
    {
        return; 
    }

    FVector2D MoveAxis = Value.Get<FVector2D>();

    //const float DeadZone = 0.15f;
    //if (FMath::Abs(MoveAxis.X) < DeadZone) MoveAxis.X = 0.f;
    //if (FMath::Abs(MoveAxis.Y) < DeadZone) MoveAxis.Y = 0.f;

    if (!IsValid(SpringArm.Get()))
    {
        UE_LOG(LogTemp, Warning, TEXT("SpringArm is invalid!"));
        return;
    }

    FRotator CameraRotation = SpringArm->GetComponentRotation();
    CameraRotation.Pitch = 0.f;
    CameraRotation.Roll = 0.f;

    FVector ForwardVector = CameraRotation.Vector();
    FVector RightVector = CameraRotation.RotateVector(FVector::RightVector);

    FVector Movement = (RightVector * -MoveAxis.X) + (ForwardVector * -MoveAxis.Y);
    Movement *= 10.0f; 

    SpringArm->SetWorldLocation(SpringArm->GetComponentLocation() + Movement);
}
void APlayerCamera::MMBPressed(const FInputActionValue& Value)
{
	bIsMiddleMouseDown = true;
}
void APlayerCamera::MMBReleased(const FInputActionValue& Value)
{
	bIsMiddleMouseDown = false;
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
		EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &APlayerCamera::MoveCamera);

		EnhancedInputComponent->BindAction(IA_MMB, ETriggerEvent::Started, this, &APlayerCamera::MMBPressed);
		EnhancedInputComponent->BindAction(IA_MMB, ETriggerEvent::Completed, this, &APlayerCamera::MMBReleased);

    }
}

void APlayerCamera::SetCameraTopDown(float ZRotation, float Height)
{
    if (IsValid(SpringArm.Get()))
    {
        // Fix: Use SetWorldRotation to ensure the camera rotates
        FRotator NewRotation = FRotator(-60.0f, ZRotation, 0.0f);
        SpringArm->SetWorldRotation(NewRotation);

        // Adjust height correctly
        SpringArm->TargetArmLength = FMath::Clamp(Height, MinZoom, MaxZoom);
    }
}