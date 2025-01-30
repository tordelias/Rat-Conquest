// Fill out your copyright notice in the Description page of Project Settings.


#include "MainHUD.h"
#include "UnitStatWidget.h"
#include "Blueprint/UserWidget.h"
#include "MainWidget.h"
#include "Rat_Conquest/Unit/PlayerUnit.h"

void AMainHUD::BeginPlay()
{
	GetOwningPlayerController()->SetInputMode(FInputModeGameAndUI());
	GetOwningPlayerController()->bShowMouseCursor = true;

	if (MainWidgetClass)
	{
		MainWidget = CreateWidget<UMainWidget>(GetWorld(), MainWidgetClass);
		if (MainWidget)
		{
			UE_LOG(LogTemp, Warning, TEXT("MainWidget created"));
			MainWidget->AddToViewport();
		}
	}

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

void AMainHUD::AddTurnImage(APlayerUnit* unit)
{
	if (MainWidget)
	{
		MainWidget->AddTurnImage(unit);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("MainWidget is null!"));
	}
}

void AMainHUD::RemoveTurnImage()
{
	if (MainWidget)
	{
		MainWidget->RemoveTurnImage();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("MainWidget is null!"));
	}
}
