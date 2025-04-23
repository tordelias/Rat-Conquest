
#include "MainWidget.h"
#include "TurnIndicatorWidget.h"
#include "Components/PanelWidget.h" // For adding widgets to a container
#include "Rat_Conquest/Unit/PlayerUnit.h" // Include the header for APlayerUnit
#include "Rat_Conquest/Widgets/UnitWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h" // Include for UTextBlock

void UMainWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Initialize the TurnIndicatorWidget when the MainWidget is constructed
    InitializeTurnIndicator();
	if(HealthBar)
	{
		HealthBar->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (bossName)
	{
		bossName->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UMainWidget::InitializeTurnIndicator()
{
    if (TurnIndicatorWidgetClass)
    {
        // Create the TurnIndicatorWidget
        TurnIndicatorWidget = CreateWidget<UTurnIndicatorWidget>(this, TurnIndicatorWidgetClass);
        if (TurnIndicatorWidget)
        {
            // Add the TurnIndicatorWidget to the MainWidget's visual hierarchy
            if (UPanelWidget* Container = Cast<UPanelWidget>(GetWidgetFromName("TurnIndicatorContainer")))
            {
                Container->AddChild(TurnIndicatorWidget);
            }

            // Optionally, initialize the TurnIndicatorWidget with data
            // TurnIndicatorWidget->Initialize();
        }
    }

	//if (UnitWidgetClass)
	//{
	//	UnitWidgetContainer = CreateWidget<UUnitWidget>(this, UnitWidgetClass);
	//	if (UnitWidgetContainer)
	//	{
	//		if (UPanelWidget* Container = Cast<UPanelWidget>(GetWidgetFromName("UnitWidgetContainer")))
	//		{
	//			Container->AddChild(UnitWidgetContainer);
	//		}
	//	}
	//}
}

void UMainWidget::AddTurnImage(APlayerUnit* unit)
{
	if (TurnIndicatorWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("Adding turn image to TurnIndicatorWidget"));
		TurnIndicatorWidget->AddTurnImage(unit);
	}
    else
	{
		UE_LOG(LogTemp, Error, TEXT("TurnIndicatorWidget is null!"));
	}
}

void UMainWidget::RemoveTurnImage()
{
	if (TurnIndicatorWidget)
	{
		TurnIndicatorWidget->RemoveTurnImage(nullptr);
	}
}
void UMainWidget::ClearTurnImages()
{
	if (TurnIndicatorWidget)
	{
		TurnIndicatorWidget->ClearTurnImages();
	}
}

void UMainWidget::UpdateTurnQueue(const TArray<APlayerUnit*>& TurnQueue)
{
	if (TurnIndicatorWidget)
	{
		TurnIndicatorWidget->UpdateTurnQueue(TurnQueue);
	}
}

void UMainWidget::RemoveUnitFromQueue(APlayerUnit* unit)
{
	if (TurnIndicatorWidget)
	{
		TurnIndicatorWidget->RemoveUnitFromQueue(unit);
	}
}

void UMainWidget::AddPlayerUnitToScreen(APlayerUnit* unit)
{
	if (UnitWidgetContainer)
	{
		UnitWidgetContainer->SetData(unit);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UnitWidget is null!"));
	}
}

void UMainWidget::HighlightPlayerUnit(TWeakObjectPtr<APlayerUnit> unit)
{
	if (UnitWidgetContainer)
	{
		UnitWidgetContainer->HighlightCurrentUnit(unit);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UnitWidget is null!"));
	}
}

void UMainWidget::SetHealthBarPercentage(float Percentage)
{
	if (HealthBar)
	{
		HealthBar->SetPercent(Percentage);
		// Hide widget if full health
		if (Percentage < 1.0f)
		{
			HealthBar->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			HealthBar->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("HealthBar is null!"));
	}
}

void UMainWidget::ShowBossHealthBar(FText Name)
{
	if (HealthBar)
	{
		HealthBar->SetVisibility(ESlateVisibility::Visible);
		if (bossName)
		{
			bossName->SetText(Name);
			bossName->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("BossName is null!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("HealthBar is null!"));
	}
}
