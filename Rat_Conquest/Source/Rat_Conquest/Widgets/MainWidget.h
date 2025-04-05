// MainWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainWidget.generated.h"

class UUnitWidget;
class UTurnIndicatorWidget; // Forward declaration

UCLASS()
class RAT_CONQUEST_API UMainWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // Called after the widget is constructed
    virtual void NativeConstruct() override;

    // Function to initialize the TurnIndicatorWidget
    UFUNCTION(BlueprintCallable, Category = "UI")
    void InitializeTurnIndicator();

	void AddTurnImage(class APlayerUnit* unit);
	void RemoveTurnImage();
    void ClearTurnImages();
	void UpdateTurnQueue(const TArray<APlayerUnit*>& TurnQueue);
	void RemoveUnitFromQueue(APlayerUnit* unit);

	void AddPlayerUnitToScreen(APlayerUnit* unit);
	void HighlightPlayerUnit(TWeakObjectPtr<APlayerUnit> unit);

protected:
    // Reference to the TurnIndicatorWidget class
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
    TSubclassOf<UTurnIndicatorWidget> TurnIndicatorWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUnitWidget> UnitWidgetClass;

    // Instance of the TurnIndicatorWidget

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widgets")
    UTurnIndicatorWidget* TurnIndicatorWidget;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widgets")
	UUnitWidget* UnitWidgetContainer;
};