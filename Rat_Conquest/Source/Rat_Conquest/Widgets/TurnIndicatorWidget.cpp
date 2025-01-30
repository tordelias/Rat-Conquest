// Fill out your copyright notice in the Description page of Project Settings.


#include "TurnIndicatorWidget.h"
#include "Rat_Conquest/Unit/PlayerUnit.h"
#include "Rat_Conquest/Widgets/UnitImageWidget.h"
#include "Components/PanelWidget.h"

void UTurnIndicatorWidget::AddTurnImage(APlayerUnit* unit)
{
	if (unit && UnitImageWidgetClass)
	{
		UUnitImageWidget* NewWidget = CreateWidget<UUnitImageWidget>(GetWorld(), UnitImageWidgetClass);
		if (NewWidget)
		{
			NewWidget->SetData(unit);
			UnitImageWidgets.Add(NewWidget);

			if (UPanelWidget* Container = Cast<UPanelWidget>(GetWidgetFromName("UnitImageContainer")))
			{
				Container->AddChild(NewWidget);
				Container->InvalidateLayoutAndVolatility();
			}
		}
	}
	UE_LOG(LogTemp, Error, TEXT("Unit added to turn indicator"));
}

void UTurnIndicatorWidget::RemoveTurnImage()
{
    if (UnitImageWidgets.Num() > 0)
    {
        // Cast the first element to UUnitImageWidget*
        UUnitImageWidget* WidgetToRemove = Cast<UUnitImageWidget>(UnitImageWidgets[0]);
        if (WidgetToRemove)
        {
            UnitImageWidgets.RemoveAt(0);

            if (UPanelWidget* Container = Cast<UPanelWidget>(GetWidgetFromName("UnitImageContainer")))
            {
                Container->RemoveChild(WidgetToRemove);
                Container->InvalidateLayoutAndVolatility(); // Refresh the container
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("UnitImageContainer is null!"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to cast UUserWidget* to UUnitImageWidget*!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No widgets to remove!"));
    }
}

void UTurnIndicatorWidget::ClearTurnImages()
{
	UnitImageWidgets.Empty();
}

void UTurnIndicatorWidget::SetCurrentUnit(APlayerUnit* Unit)
{
}
