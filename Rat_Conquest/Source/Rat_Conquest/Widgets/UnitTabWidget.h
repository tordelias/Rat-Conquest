#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UnitTabWidget.generated.h"

class UTextBlock;
class APlayerUnit;
class UItemToolTipWidget;
class UButton;
class UImage;

UCLASS()
class RAT_CONQUEST_API UUnitTabWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Icon")
	UTexture2D* DefaultIcon;

	// Buttons that will wrap the item images
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* ItemButton1;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* ItemButton2;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* ItemButton3;

	// Item images inside the buttons
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* ItemImage1;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* ItemImage2;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* ItemImage3;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* Attack;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* Damage;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* Range;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* Health;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* Defense;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* Speed;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* initiativ;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* ItemName1;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* ItemName2;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* ItemName3;

	// The unit whose items are displayed
	UPROPERTY()
	APlayerUnit* Unit;

	// Tooltip Widget
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tooltip")
	TSubclassOf<UItemToolTipWidget> ItemToolTipWidgetClass;

	// Instance of the tooltip widget
	UPROPERTY()
	UItemToolTipWidget* ToolTipInstance;

	// Function to initialize unit data
	void SetData(APlayerUnit* _Unit);

	// Called when the widget is constructed
	virtual void NativeConstruct() override;

	// Functions for button hover events
	UFUNCTION()
	void OnItemButton1Hovered();

	UFUNCTION()
	void OnItemButton2Hovered();

	UFUNCTION()
	void OnItemButton3Hovered();

	UFUNCTION()
	void OnItemButtonUnhovered();


	bool bButton1Active = false;
	bool bButton2Active = false;
	bool bButton3Active = false;
};
