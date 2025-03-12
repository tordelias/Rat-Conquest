// UnitImageWidget.cpp
#include "UnitImageWidget.h"
#include "Rat_Conquest/Unit/PlayerUnit.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/SizeBox.h"
#include "ItemToolTipWidget.h"


void UUnitImageWidget::SetData(APlayerUnit* _Unit)
{
    if (_Unit)
    {
        this->Unit = _Unit;

        // Set the unit's image (assuming APlayerUnit has a GetUnitTexture() function)
        if (UnitImage && _Unit->UnitIcon)
        {
            UnitImage->SetBrushFromTexture(_Unit->UnitIcon);
        }
    }
    else
    {
		// Set the default image
		if (UnitImage && DefaultIcon)
		{
			UnitImage->SetBrushFromTexture(DefaultIcon);
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
            SizeBox->SetWidthOverride(100.f);
            SizeBox->SetHeightOverride(100.f);
        }
    }
}