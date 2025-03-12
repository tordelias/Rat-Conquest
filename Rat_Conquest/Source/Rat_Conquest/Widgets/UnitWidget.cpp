#include "UnitWidget.h"
#include "Rat_Conquest/Unit/PlayerUnit.h"
#include "Components/Button.h"
#include "Rat_Conquest/Widgets/UnitImageWidget.h"
#include "Rat_Conquest/Widgets/UnitTabWidget.h"


void UUnitWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Ensure the unit images are initialized
	UnitImageWidget1->SetData(nullptr);
	UnitImageWidget2->SetData(nullptr);
	UnitImageWidget3->SetData(nullptr);
	UnitImageWidget4->SetData(nullptr);

	// Bind the click events
	if (UnitImageButton1)
	{
		UnitImageButton1->OnClicked.AddDynamic(this, &UUnitWidget::OnUnitImage1Clicked);
	}
	if (UnitImageButton2)
	{
		UnitImageButton2->OnClicked.AddDynamic(this, &UUnitWidget::OnUnitImage2Clicked);
	}
	if (UnitImageButton3)
	{
		UnitImageButton3->OnClicked.AddDynamic(this, &UUnitWidget::OnUnitImage3Clicked);
	}
	if (UnitImageButton4)
	{
		UnitImageButton4->OnClicked.AddDynamic(this, &UUnitWidget::OnUnitImage4Clicked);
	}

	// Initially hide the UnitTabWidget
	if (UnitTabWidget1)
	{
		UnitTabWidget1->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UUnitWidget::OnUnitImage1Clicked()
{
	OnUnitImageClicked(UnitImageWidget1);
}

void UUnitWidget::OnUnitImage2Clicked()
{
	OnUnitImageClicked(UnitImageWidget2);
}

void UUnitWidget::OnUnitImage3Clicked()
{
	OnUnitImageClicked(UnitImageWidget3);
}

void UUnitWidget::OnUnitImage4Clicked()
{
	OnUnitImageClicked(UnitImageWidget4);
}

void UUnitWidget::SetData(APlayerUnit* _Unit)
{
	if (!_Unit) return;

	// Assign the unit to the first available UnitImageWidget
	UUnitImageWidget* AvailableWidget = nullptr;

	if (!UnitImageWidget1->Unit) AvailableWidget = UnitImageWidget1;
	else if (!UnitImageWidget2->Unit) AvailableWidget = UnitImageWidget2;
	else if (!UnitImageWidget3->Unit) AvailableWidget = UnitImageWidget3;
	else if (!UnitImageWidget4->Unit) AvailableWidget = UnitImageWidget4;
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No more room for units in the unit widget!"));
		return;
	}

	UnitTabWidget1->SetData(_Unit);

	// Assign unit and allow items to be shown
	AvailableWidget->ShowUnitItems = true;
	AvailableWidget->SetData(_Unit);
	PlayerUnits.Add(_Unit);
}

void UUnitWidget::OnUnitImageClicked(UUnitImageWidget* ClickedWidget)
{
	if (!ClickedWidget || !UnitTabWidget1) return;

	// If clicking the same unit, toggle visibility
	if (ActiveUnit == ClickedWidget->Unit)
	{
		ESlateVisibility NewVisibility = (UnitTabWidget1->GetVisibility() == ESlateVisibility::Visible)
			? ESlateVisibility::Collapsed
			: ESlateVisibility::Visible;

		UnitTabWidget1->SetVisibility(NewVisibility);
	}
	else
	{
		// Update the unit tab with the new unit
		UnitTabWidget1->SetData(ClickedWidget->Unit);
		UnitTabWidget1->SetVisibility(ESlateVisibility::Visible);
	}

	// Update the currently active unit
	ActiveUnit = ClickedWidget->Unit;
}
