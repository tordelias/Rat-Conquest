// UnitImageWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UnitImageWidget.generated.h"

class APlayerUnit;

UCLASS()
class RAT_CONQUEST_API UUnitImageWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // Bind these to the widget blueprint
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UImage* UnitImage;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UTextBlock* UnitName;

    // Function to initialize the widget with unit data
    UFUNCTION(BlueprintCallable)
    void SetData(APlayerUnit* Unit);

    UFUNCTION(BlueprintCallable)
    void SetSize(bool bIsFirstElement);

    //// Reference to the unit this widget represents
    //UPROPERTY()
    //APlayerUnit* Unit;
};