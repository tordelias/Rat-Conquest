// UnitImageWidget.cpp
#include "UnitImageWidget.h"
#include "Rat_Conquest/Unit/PlayerUnit.h" // Include the header for APlayerUnit
#include "Components/Image.h"
#include "Components/TextBlock.h"

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