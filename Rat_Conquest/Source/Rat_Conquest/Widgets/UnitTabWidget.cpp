#include "UnitTabWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "ItemToolTipWidget.h"
#include "Components/TextBlock.h"
#include "Rat_Conquest/Unit/PlayerUnit.h"
#include "Rat_Conquest/Items/Item.h"
#include "Rat_Conquest/Items/ItemBase.h"



void UUnitTabWidget::SetData(APlayerUnit* _Unit)
{
    if (!_Unit) return;

    Unit = _Unit;

    // Create the tooltip once
    if (!ToolTipInstance && ItemToolTipWidgetClass)
    {
        ToolTipInstance = CreateWidget<UItemToolTipWidget>(GetWorld(), ItemToolTipWidgetClass);
    }
    if (Unit->Weapon)
    {
		ItemImage1->SetBrushFromTexture(Unit->Weapon->ItemDataB->ItemAssetData.Icon);
		ItemName1->SetText(Unit->Weapon->ItemDataB->ItemTextData.Name);
		bButton1Active = true;
    }
    else
	{
		ItemImage1->SetBrushFromTexture(DefaultIcon);
		bButton1Active = false;
	}
    if (Unit->Armour)
    {
        ItemImage2->SetBrushFromTexture(Unit->Armour->ItemDataB->ItemAssetData.Icon);
        ItemName2->SetText(Unit->Armour->ItemDataB->ItemTextData.Name);

		bButton2Active = true;
    }
    else
    {
        ItemImage1->SetBrushFromTexture(DefaultIcon);
		bButton2Active = false;
    }
	if (Unit->Artifact)
	{
		ItemImage3->SetBrushFromTexture(Unit->Artifact->ItemDataB->ItemAssetData.Icon);
		ItemName3->SetText(Unit->Artifact->ItemDataB->ItemTextData.Name);
		bButton3Active = true;
	}
    else
    {
        ItemImage1->SetBrushFromTexture(DefaultIcon);
		bButton3Active = false;
    }

    if (Attack)     Attack->SetText(FText::AsNumber(Unit->Attack));
    if (Damage)     Damage->SetText(FText::AsNumber(Unit->Damage));
    if (Range)      Range->SetText(FText::AsNumber(Unit->AttackRange));
    if (Health)     Health->SetText(FText::AsNumber(Unit->Health));
    if (Defense)    Defense->SetText(FText::AsNumber(Unit->Defence));
    if (Speed)      Speed->SetText(FText::AsNumber(Unit->MovementSpeed));
    if (initiativ)  initiativ->SetText(FText::AsNumber(Unit->Initiative));
}

void UUnitTabWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Bind hover events for each item button
    if (ItemButton1) ItemButton1->OnHovered.AddDynamic(this, &UUnitTabWidget::OnItemButton1Hovered);
    if (ItemButton2) ItemButton2->OnHovered.AddDynamic(this, &UUnitTabWidget::OnItemButton2Hovered);
    if (ItemButton3) ItemButton3->OnHovered.AddDynamic(this, &UUnitTabWidget::OnItemButton3Hovered);

    if (ItemButton1) ItemButton1->OnUnhovered.AddDynamic(this, &UUnitTabWidget::OnItemButtonUnhovered);
    if (ItemButton2) ItemButton2->OnUnhovered.AddDynamic(this, &UUnitTabWidget::OnItemButtonUnhovered);
    if (ItemButton3) ItemButton3->OnUnhovered.AddDynamic(this, &UUnitTabWidget::OnItemButtonUnhovered);

	ItemImage1->SetBrushFromTexture(DefaultIcon);
	ItemImage2->SetBrushFromTexture(DefaultIcon);
	ItemImage3->SetBrushFromTexture(DefaultIcon);
}

// Show tooltip for Item 1
void UUnitTabWidget::OnItemButton1Hovered()
{
    if (ToolTipInstance && Unit && bButton1Active)
    {
        UItemToolTipWidget* ItemToolTip = Cast<UItemToolTipWidget>(ToolTipInstance);
        if (ItemToolTip)
        {
            ItemToolTip->SetData(Unit->Weapon); // Set Item 1 data
            SetToolTip(ToolTipInstance);
        }
    }
}

// Show tooltip for Item 2
void UUnitTabWidget::OnItemButton2Hovered()
{
    if (ToolTipInstance && Unit && bButton2Active)
    {
        UItemToolTipWidget* ItemToolTip = Cast<UItemToolTipWidget>(ToolTipInstance);
        if (ItemToolTip)
        {
            ItemToolTip->SetData(Unit->Armour); // Set Item 2 data
            SetToolTip(ToolTipInstance);
        }
    }
}

// Show tooltip for Item 3
void UUnitTabWidget::OnItemButton3Hovered()
{
    if (ToolTipInstance && Unit && bButton3Active)
    {
        UItemToolTipWidget* ItemToolTip = Cast<UItemToolTipWidget>(ToolTipInstance);
        if (ItemToolTip)
        {
            ItemToolTip->SetData(Unit->Artifact); // Set Item 3 data
            SetToolTip(ToolTipInstance);
        }
    }
}

// Hide tooltip when the mouse leaves any item button
void UUnitTabWidget::OnItemButtonUnhovered()
{
    SetToolTip(nullptr);
}
