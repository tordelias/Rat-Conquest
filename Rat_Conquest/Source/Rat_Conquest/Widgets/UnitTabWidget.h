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
	TWeakObjectPtr<UTexture2D> DefaultIcon;

	// Buttons that will wrap the item images
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TWeakObjectPtr<UButton> ItemButton1;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TWeakObjectPtr<UButton> ItemButton2;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TWeakObjectPtr<UButton> ItemButton3;

	// Item images inside the buttons
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TWeakObjectPtr<UImage> ItemImage1;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TWeakObjectPtr<UImage> ItemImage2;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TWeakObjectPtr<UImage> ItemImage3;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TWeakObjectPtr<UTextBlock> Attack;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TWeakObjectPtr<UTextBlock> Damage;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TWeakObjectPtr<UTextBlock> Range;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TWeakObjectPtr<UTextBlock> Health;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TWeakObjectPtr<UTextBlock> Defense;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TWeakObjectPtr<UTextBlock> Speed;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TWeakObjectPtr<UTextBlock> initiativ;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TWeakObjectPtr<UTextBlock> ItemName1;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TWeakObjectPtr<UTextBlock> ItemName2;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TWeakObjectPtr<UTextBlock> ItemName3;

	// The unit whose items are displayed
	UPROPERTY()
	TWeakObjectPtr<APlayerUnit> Unit;

	// Tooltip Widget
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tooltip")
	TSubclassOf<UItemToolTipWidget> ItemToolTipWidgetClass;

	// Instance of the tooltip widget
	UPROPERTY()
	TObjectPtr<UItemToolTipWidget> ToolTipInstance;

	// Function to initialize unit data
	void SetData(TWeakObjectPtr<APlayerUnit> _Unit);

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
