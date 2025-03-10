// Fill out your copyright notice in the Description page of Project Settings.


#include "TurnIndicatorWidget.h"
#include "Rat_Conquest/Unit/PlayerUnit.h"
#include "Rat_Conquest/Widgets/UnitImageWidget.h"
#include "Components/PanelWidget.h"

void UTurnIndicatorWidget::AddTurnImage(APlayerUnit* unit)
{
    if (unit && UnitImageWidgetClass)
    {
        // Only add the unit if it's not already in the TurnQueue
        if (!TurnQueue.Contains(unit))
        {
            TurnQueue.Add(unit);  // Add the unit to the TurnQueue
        }

        UUnitImageWidget* NewWidget = CreateWidget<UUnitImageWidget>(GetWorld(), UnitImageWidgetClass);
        if (NewWidget)
        {
            NewWidget->SetData(unit);  // Set the unit data to the widget
            UnitImageWidgets.Add(NewWidget);  // Add it to the list of widgets

            // Resize the first element (bigger size for the first element)
            if (UnitImageWidgets.Num() == 1)
            {
                NewWidget->SetSize(true);
            }
            else
            {
                NewWidget->SetSize(false);
            }

            // Get the panel container where widgets will be added
            if (UPanelWidget* Container = Cast<UPanelWidget>(GetWidgetFromName("UnitImageContainer")))
            {
                Container->AddChild(NewWidget);  // Add the new widget to the container
                Container->InvalidateLayoutAndVolatility();  // Refresh the layout
            }
        }
    }
    UE_LOG(LogTemp, Error, TEXT("Unit added to turn indicator"));
}


void UTurnIndicatorWidget::RemoveTurnImage()
{
    if (UnitImageWidgets.Num() > 0)
    {
        // Get the first widget in the list to remove
        UUnitImageWidget* WidgetToRemove = Cast<UUnitImageWidget>(UnitImageWidgets[0]);
        if (WidgetToRemove)
        {
            UnitImageWidgets.RemoveAt(0);  // Remove it from the list

            // Get the container to remove the widget
            if (UPanelWidget* Container = Cast<UPanelWidget>(GetWidgetFromName("UnitImageContainer")))
            {
                Container->RemoveChild(WidgetToRemove);  // Remove from container
                Container->InvalidateLayoutAndVolatility();  // Refresh the container
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("UnitImageContainer is null!"));
            }

            // Resize the new first element (if there are any left)
            if (UnitImageWidgets.Num() > 0)
            {
                UUnitImageWidget* NewFirstWidget = Cast<UUnitImageWidget>(UnitImageWidgets[0]);
                if (NewFirstWidget)
                {
                    NewFirstWidget->SetSize(true); // Resize the new first element
                }
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
    UnitImageWidgets.Empty();  // Clear all unit images from the widget list
}

void UTurnIndicatorWidget::SetCurrentUnit(APlayerUnit* Unit)
{
    // This method could be useful for visualizing the currently selected unit
}

void UTurnIndicatorWidget::UpdateTurnQueue(const TArray<APlayerUnit*>& sentTurnQueue)
{
    // Add unit widgets to the UI for any units that are currently in the TurnQueue
    for (APlayerUnit* Unit : sentTurnQueue)
    {
        if (Unit)
        {
            // Add the unit image if it's not already in the queue (to avoid duplicates)
            AddTurnImage(Unit);
        }
    }

    // Optionally resize the first widget (larger size for the first unit)
    if (UnitImageWidgets.Num() > 0)
    {
        UUnitImageWidget* NewFirstWidget = Cast<UUnitImageWidget>(UnitImageWidgets[0]);
        if (NewFirstWidget)
        {
            NewFirstWidget->SetSize(true); // Resize the new first element
        }
    }

    // Update the container layout to ensure it reflects the changes
    if (UPanelWidget* Container = Cast<UPanelWidget>(GetWidgetFromName("UnitImageContainer")))
    {
        Container->InvalidateLayoutAndVolatility();  // Refresh the layout of the container
    }
}

void UTurnIndicatorWidget::RemoveUnitFromQueue(APlayerUnit* unit)
{
    // Remove unit from the turn queue
    if (TurnQueue.RemoveSingleSwap(unit) > 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Unit removed from turn queue"));

        // Remove the unit's specific image widget from the UI
        RemoveTurnImage();  // This will remove the first widget, assuming that's the correct one to remove

        // Ensure the container gets updated with the remaining widgets
        if (UPanelWidget* Container = Cast<UPanelWidget>(GetWidgetFromName("UnitImageContainer")))
        {
            Container->InvalidateLayoutAndVolatility();  // Refresh the layout of the container
        }

        // Update the UI after the removal
        UpdateTurnQueue(TurnQueue);  // Ensure the UI is in sync with the turn queue
    }
}





