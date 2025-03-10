
#include "MainWidget.h"
#include "TurnIndicatorWidget.h"
#include "Components/PanelWidget.h" // For adding widgets to a container
#include "Rat_Conquest/Unit/PlayerUnit.h" // Include the header for APlayerUnit

void UMainWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Initialize the TurnIndicatorWidget when the MainWidget is constructed
    InitializeTurnIndicator();
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
		TurnIndicatorWidget->RemoveTurnImage();
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
