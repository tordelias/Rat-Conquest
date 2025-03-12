// UnitImageWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UnitImageWidget.generated.h"

class UItemToolTipWidget;
class APlayerUnit;

UCLASS()
class RAT_CONQUEST_API UUnitImageWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // Bind these to the widget blueprint
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UImage* UnitImage;

	//default image when no unit is present
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Icon")
	UTexture2D* DefaultIcon;

    //UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    //class UTextBlock* UnitName;

    // Function to initialize the widget with unit data
    UFUNCTION(BlueprintCallable)
    void SetData(APlayerUnit* Unit);

    UFUNCTION(BlueprintCallable)
    void SetSize(bool bIsFirstElement);


    //// Reference to the unit this widget represents
    UPROPERTY()
    APlayerUnit* Unit;

	bool ShowUnitItems = false; // Show the unit's items in the widget
};