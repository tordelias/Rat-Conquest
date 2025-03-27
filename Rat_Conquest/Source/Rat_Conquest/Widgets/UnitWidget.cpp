#include "UnitWidget.h"
#include "Rat_Conquest/Unit/PlayerUnit.h"
#include "Components/Button.h"
#include "Rat_Conquest/Widgets/UnitImageWidget.h"
#include "Rat_Conquest/Widgets/UnitTabWidget.h"
#include "Components/ProgressBar.h"


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
	HealthBar1->SetVisibility(ESlateVisibility::Collapsed);
	HealthBar2->SetVisibility(ESlateVisibility::Collapsed);
	HealthBar3->SetVisibility(ESlateVisibility::Collapsed);
	HealthBar4->SetVisibility(ESlateVisibility::Collapsed);

	Units.SetNum(4); 
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

void UUnitWidget::OnUnitHealthChanged()
{
	for (auto Unit : Units)
	{
		if (!Unit) continue;
		// Find the corresponding health bar
		UProgressBar* CorrespondingHealthBar = nullptr;
		if (Unit == UnitImageWidget1->Unit)
		{
			CorrespondingHealthBar = HealthBar1;
		}
		else if (Unit == UnitImageWidget2->Unit)
		{
			CorrespondingHealthBar = HealthBar2;
		}
		else if (Unit == UnitImageWidget3->Unit)
		{
			CorrespondingHealthBar = HealthBar3;
		}
		else if (Unit == UnitImageWidget4->Unit)
		{
			CorrespondingHealthBar = HealthBar4;
		}
		// Update the health bar
		if (CorrespondingHealthBar)
		{
			float HealthPercentage = static_cast<float>(Unit->Health) / static_cast<float>(Unit->maxHealth);
			CorrespondingHealthBar->SetPercent(HealthPercentage);
		}
	}
}

void UUnitWidget::SetData(APlayerUnit* _Unit)
{
	if (!_Unit) return;

	// Assign the unit to the first available UnitImageWidget
	UUnitImageWidget* AvailableWidget = nullptr;
	UProgressBar* CorrespondingHealthBar = nullptr;

	if (!UnitImageWidget1->Unit)
	{
		AvailableWidget = UnitImageWidget1;
		CorrespondingHealthBar = HealthBar1;
		Units[0] = _Unit;
	}
	else if (!UnitImageWidget2->Unit && _Unit != Units[0])
	{
		AvailableWidget = UnitImageWidget2;
		CorrespondingHealthBar = HealthBar2;
		Units[1] = _Unit;
	}
	else if (!UnitImageWidget3->Unit && _Unit != Units[1])
	{
		AvailableWidget = UnitImageWidget3;
		CorrespondingHealthBar = HealthBar3;
		Units[2] = _Unit;
	}
	else if (!UnitImageWidget4->Unit && _Unit != Units[2])
	{
		AvailableWidget = UnitImageWidget4;
		CorrespondingHealthBar = HealthBar4;
		Units[3] = _Unit;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No more room for units in the unit widget!"));
		return;
	}

	// Assign unit and allow items to be shown
	AvailableWidget->ShowUnitItems = true;
	AvailableWidget->SetData(_Unit);
	PlayerUnits.Add(_Unit);

	// Show health bar if the unit is valid
	if (CorrespondingHealthBar)
	{
		CorrespondingHealthBar->SetVisibility(ESlateVisibility::Visible);
		float HealthPercentage = static_cast<float>(_Unit->Health) / static_cast<float>(_Unit->maxHealth);

		CorrespondingHealthBar->SetPercent(HealthPercentage);

		_Unit->OnHealthChanged.AddDynamic(this, &UUnitWidget::OnUnitHealthChanged);
	}
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
		UnitTabWidget1->SetData(ClickedWidget->Unit);
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
