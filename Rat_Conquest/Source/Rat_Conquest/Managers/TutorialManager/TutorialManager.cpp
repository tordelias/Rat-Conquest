#include "TutorialManager.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Rat_Conquest/Widgets/TutorialWidget.h"

// Sets default values
ATutorialManager::ATutorialManager()
{
    PrimaryActorTick.bCanEverTick = false;
    CurrentStepIndex = 0;
}

// Called when the game starts or when spawned
void ATutorialManager::BeginPlay()
{
    Super::BeginPlay();

    if (TutorialWidgetClass)
    {
        TutorialWidget = CreateWidget<UTutorialWidget>(GetWorld(), TutorialWidgetClass);
        if (TutorialWidget)
        {
            TutorialWidget->AddToViewport(10);
            TutorialWidget->TutorialManager = this; // Assign reference to TutorialManager
            DisplayCurrentStep();
        }
    }
}


// Called every frame (not needed, so it's disabled)
void ATutorialManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// Display the current tutorial step in the UI
void ATutorialManager::DisplayCurrentStep()
{
    if (TutorialSteps.IsValidIndex(CurrentStepIndex) && TutorialWidget)
    {
        TutorialWidget->SetText(FText::FromString(TutorialSteps[CurrentStepIndex].InstructionText));
    }
}

// Check if the player's action matches the required tutorial action
void ATutorialManager::CheckAction(FName ActionName)
{
    if (TutorialSteps.IsValidIndex(CurrentStepIndex))
    {
        if (TutorialSteps[CurrentStepIndex].bRequiresAction && TutorialSteps[CurrentStepIndex].RequiredAction == ActionName)
        {
            AdvanceTutorial();
        }
    }
}

// Move to the next tutorial step
void ATutorialManager::AdvanceTutorial()
{
    CurrentStepIndex++;

    if (TutorialSteps.IsValidIndex(CurrentStepIndex))
    {
        DisplayCurrentStep();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Tutorial Completed!"));
    }
}
