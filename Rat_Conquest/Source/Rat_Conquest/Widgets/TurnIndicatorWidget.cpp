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

            // Resize the first element
            if (UnitImageWidgets.Num() == 1)
            {
                NewWidget->SetSize(true); 
            }
            else
            {
                NewWidget->SetSize(false); 
            }

            if (UPanelWidget* Container = Cast<UPanelWidget>(GetWidgetFromName("UnitImageContainer")))
            {
                Container->AddChild(NewWidget);
                Container->InvalidateLayoutAndVolatility();
            }
        }
    }
    UE_LOG(LogTemp, Error, TEXT("Unit added to turn indicator"));
}

void UTurnIndicatorWidget::RemoveTurnImage(APlayerUnit* unit)
{
    if (UnitImageWidgets.Num() > 0 && unit == nullptr)
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

            // Resize the new first element (if any)
            if (UnitImageWidgets.Num() > 0)
            {
                UUnitImageWidget* NewFirstWidget = Cast<UUnitImageWidget>(UnitImageWidgets[0]);
                if (NewFirstWidget)
                {
                    NewFirstWidget->SetSize(true); // Make the new first element bigger
                }
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to cast UUserWidget* to UUnitImageWidget*!"));
        }
    }
    else if (unit && UnitImageWidgets.Num() > 0)
    {
		for (int i = 0; i < UnitImageWidgets.Num(); i++)
		{
			UUnitImageWidget* WidgetToRemove = Cast<UUnitImageWidget>(UnitImageWidgets[i]);
			if (WidgetToRemove && WidgetToRemove->Unit == unit)
			{
				UnitImageWidgets.RemoveAt(i);
				if (UPanelWidget* Container = Cast<UPanelWidget>(GetWidgetFromName("UnitImageContainer")))
				{
					Container->RemoveChild(WidgetToRemove);
					Container->InvalidateLayoutAndVolatility(); // Refresh the container
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("UnitImageContainer is null!"));
				}
				// Resize the new first element (if any)
				if (UnitImageWidgets.Num() > 0)
				{
					UUnitImageWidget* NewFirstWidget = Cast<UUnitImageWidget>(UnitImageWidgets[0]);
					if (NewFirstWidget)
					{
						NewFirstWidget->SetSize(true); // Make the new first element bigger
					}
				}
			}
		}
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No widgets to remove!"));
    }
}

void UTurnIndicatorWidget::ClearTurnImages()
{
	if (UnitImageWidgets.Num() > 0)
	{
		for (UUserWidget* Widget : UnitImageWidgets)
		{
			if (UPanelWidget* Container = Cast<UPanelWidget>(GetWidgetFromName("UnitImageContainer")))
			{
				Container->RemoveChild(Widget);
			}
		}
		UnitImageWidgets.Empty();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No widgets to clear!"));
	}
}

void UTurnIndicatorWidget::SetCurrentUnit(APlayerUnit* Unit)
{
}

void UTurnIndicatorWidget::UpdateTurnQueue(const TArray<APlayerUnit*>& TurnQueue)
{
    ClearTurnImages(); // Clear old turn order

    for (APlayerUnit* Unit : TurnQueue)
    {
        if (Unit)
        {
            AddTurnImage(Unit); // Add new turn order
        }
    }
}

void UTurnIndicatorWidget::RemoveUnitFromQueue(APlayerUnit* unit)
{
    RemoveTurnImage(unit); 

}



