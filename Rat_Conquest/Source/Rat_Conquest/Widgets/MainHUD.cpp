// Fill out your copyright notice in the Description page of Project Settings.


#include "MainHUD.h"
#include "UnitStatWidget.h"
#include "Blueprint/UserWidget.h"

void AMainHUD::BeginPlay()
{
	GetOwningPlayerController()->SetInputMode(FInputModeGameAndUI());
	GetOwningPlayerController()->bShowMouseCursor = true;

	if (UnitWidgetclass)
	{
		StatWidget = CreateWidget<UUnitStatWidget>(GetWorld(), UnitWidgetclass);
		if (StatWidget)
		{
			StatWidget->AddToViewport();
			StatWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void AMainHUD::ShowStatWidget()
{
	if (StatWidget)
	{
		StatWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void AMainHUD::CloseStatWidget()
{
	if (StatWidget)
	{
		StatWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void AMainHUD::UpdateStatWidget(FInteractableData* data)
{
	if (StatWidget)
	{
		StatWidget->UpdateWidget(data);
	}
}
