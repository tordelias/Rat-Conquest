#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TutorialManager.generated.h"

USTRUCT(BlueprintType)
struct FTutorialStep
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString InstructionText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bRequiresAction;  // Whether the step requires player input

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName RequiredAction;  // Name of the action required to complete this step
};

UCLASS()
class RAT_CONQUEST_API ATutorialManager : public AActor
{
    GENERATED_BODY()

public:
    ATutorialManager();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    void AdvanceTutorial();  // Move to the next step
    void CheckAction(FName ActionName);  // Verify if the correct action was taken

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> TutorialWidgetClass;


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial")
    TArray<FTutorialStep> TutorialSteps;

    UPROPERTY(BlueprintReadOnly, Category = "Tutorial")
    int32 CurrentStepIndex;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    class UTutorialWidget* TutorialWidget;

    void DisplayCurrentStep();  // Show the current tutorial instruction
};
