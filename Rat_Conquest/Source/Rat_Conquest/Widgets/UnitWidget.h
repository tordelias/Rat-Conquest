#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UnitWidget.generated.h"

class UProgressBar;
class UUnitTabWidget;
class UUnitImageWidget;
class APlayerUnit;
class UButton; // Include UButton for handling clicks

UCLASS()
class RAT_CONQUEST_API UUnitWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<APlayerUnit*> PlayerUnits; // Max 4 units

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUnitImageWidget> UnitImageWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUnitTabWidget> UnitTabWidgetClass;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Unit Widget")
	UUnitImageWidget* UnitImageWidget1;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Unit Widget")
	UUnitImageWidget* UnitImageWidget2;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Unit Widget")
	UUnitImageWidget* UnitImageWidget3;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Unit Widget")
	UUnitImageWidget* UnitImageWidget4;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Unit Widget")
	UUnitTabWidget* UnitTabWidget1;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Unit Widget")
	UProgressBar* HealthBar1;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Unit Widget")
	UProgressBar* HealthBar2;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Unit Widget")
	UProgressBar* HealthBar3;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Unit Widget")
	UProgressBar* HealthBar4;

	// Buttons inside the UnitImageWidgets for handling clicks
	UPROPERTY(meta = (BindWidget))
	UButton* UnitImageButton1;

	UPROPERTY(meta = (BindWidget))
	UButton* UnitImageButton2;

	UPROPERTY(meta = (BindWidget))
	UButton* UnitImageButton3;

	UPROPERTY(meta = (BindWidget))
	UButton* UnitImageButton4;

	// Tracks the currently active unit in the UnitTabWidget
	APlayerUnit* ActiveUnit;

	TArray<APlayerUnit*> Units;

	virtual void NativeConstruct() override;

	void SetData(APlayerUnit* _Unit);

	UFUNCTION()
	void OnUnitImageClicked(UUnitImageWidget* ClickedWidget);

	UFUNCTION()
	void OnUnitImage1Clicked();

	UFUNCTION()
	void OnUnitImage2Clicked();

	UFUNCTION()
	void OnUnitImage3Clicked();

	UFUNCTION()
	void OnUnitImage4Clicked();

	UFUNCTION()
	void OnUnitHealthChanged();

	UFUNCTION()
	void HighlightCurrentUnit(TWeakObjectPtr<APlayerUnit> CurrentUnit);


};
