// Fill out your copyright notice in the Description page of Project Settings.


#include "MainHUD.h"
#include "UnitStatWidget.h"
#include "Blueprint/UserWidget.h"
#include "MainWidget.h"
#include "MutationWidget.h"
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
			StatWidget->AddToViewport(1);
			StatWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (MutationWidgetClass)
	{
		MutationWidget = CreateWidget<UMutationWidget>(GetWorld(), MutationWidgetClass);
		if (MutationWidget)
		{
			MutationWidget->AddToViewport();
			MutationWidget->SetVisibility(ESlateVisibility::Hidden);
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

void AMainHUD::ShowMutationWidget()
{
	if (MutationWidget)
	{
		MutationWidget->SetVisibility(ESlateVisibility::Visible);
		if (MainWidget)
		{
			MainWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("MutationWidget is null!"));
	}
}

void AMainHUD::CloseMutationWidget()
{
	if (MutationWidget)
	{
		MutationWidget->SetVisibility(ESlateVisibility::Collapsed);
		if (MainWidget)
		{
			MainWidget->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("MutationWidget is null!"));
	}
}

void AMainHUD::UpdateMutationWidget(TArray<int> statsC1, TArray<int> statsC2, TArray<int> statsC3, FName UnitName, APlayerUnit* unit)
{
	if (MutationWidget)
	{
		if (unit)
		{
			UE_LOG(LogTemp, Error, TEXT("Unit is sendt to mutationWidget"))
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Unit is null in MainHUD!"));
		}

		MutationWidget->SetData(statsC1, statsC2, statsC3, UnitName, unit);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("MutationWidget is null!"));
	}
}
