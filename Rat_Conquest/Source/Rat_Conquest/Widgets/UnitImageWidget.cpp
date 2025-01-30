// UnitImageWidget.cpp
#include "UnitImageWidget.h"
#include "Rat_Conquest/Unit/PlayerUnit.h" // Include the header for APlayerUnit
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/SizeBox.h"

void UUnitImageWidget::SetData(APlayerUnit* Unit)
{
    if (Unit)
    {
       // this->Unit = Unit;

        // Set the unit's image (assuming APlayerUnit has a GetUnitTexture() function)
        if (UnitImage && Unit->UnitIcon)
        {
            UnitImage->SetBrushFromTexture(Unit->UnitIcon);
        }

        // Set the unit's name (assuming APlayerUnit has a GetUnitName() function)
        if (UnitName)
        {
            UnitName->SetText(Unit->InstanceInteractableData.UnitName);
        }
    }
}

void UUnitImageWidget::SetSize(bool bIsFirstElement)
{
    if (bIsFirstElement)
    {
        // Make the first element bigger
        if (USizeBox* SizeBox = Cast<USizeBox>(GetWidgetFromName("SizeBox")))
        {
            SizeBox->SetWidthOverride(150.f); 
            SizeBox->SetHeightOverride(150.f);
        }
    }
    else
    {
        // Reset the size for other elements
        if (USizeBox* SizeBox = Cast<USizeBox>(GetWidgetFromName("SizeBox")))
        {
            SizeBox->ClearWidthOverride();
            SizeBox->ClearHeightOverride();
        }
    }
}
