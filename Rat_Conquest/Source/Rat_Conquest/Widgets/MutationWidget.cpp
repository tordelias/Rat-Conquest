// Fill out your copyright notice in the Description page of Project Settings.


#include "MutationWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Rat_Conquest/Unit/PlayerUnit.h"

void UMutationWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (Choice1)
	{
		Choice1->OnClicked.AddDynamic(this, &UMutationWidget::OnChoice1Clicked);
	}
	if (Choice2)
	{
		Choice2->OnClicked.AddDynamic(this, &UMutationWidget::OnChoice2Clicked);
	}
	if (Choice3)
	{
		Choice3->OnClicked.AddDynamic(this, &UMutationWidget::OnChoice3Clicked);
	}
}

void UMutationWidget::SetData(TArray<int> statsC1, TArray<int> statsC2, TArray<int> statsC3, FName NewUnitName, APlayerUnit* currentUnit )
{
	//speed = 0, attack = 1, defense = 2, health = 3

	if (this->UnitName)
	{
		this->UnitName->SetText(FText::FromName(NewUnitName));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UnitName is null in MutationWidget!"));
	}

	if (statsC1.Num() < 4 || statsC2.Num() < 4 || statsC3.Num() < 4)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid stats array size!"));
		return;
	}


	auto SetStatText = [](UTextBlock* TextBlock, int Value)
		{
			if (!TextBlock) return;

			// Set text
			TextBlock->SetText(FText::FromString(FString::FromInt(Value)));

			// Set color based on value
			if (Value > 0)
			{
				TextBlock->SetColorAndOpacity(FSlateColor(FLinearColor::Green));
			}
			else if (Value < 0)
			{
				TextBlock->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
			}
		};

	// Set stats for each column
	SetStatText(this->SpeedC1, statsC1[0]);
	SetStatText(this->DamageC1, statsC1[1]);
	SetStatText(this->DefenseC1, statsC1[2]);
	SetStatText(this->HealthC1, statsC1[3]);

	SetStatText(this->SpeedC2, statsC2[0]);
	SetStatText(this->DamageC2, statsC2[1]);
	SetStatText(this->DefenseC2, statsC2[2]);
	SetStatText(this->HealthC2, statsC2[3]);

	SetStatText(this->SpeedC3, statsC3[0]);
	SetStatText(this->DamageC3, statsC3[1]);
	SetStatText(this->DefenseC3, statsC3[2]);
	SetStatText(this->HealthC3, statsC3[3]);

	unit = currentUnit;
	C1 = statsC1;
	C2 = statsC2;
	C3 = statsC3;
}

void UMutationWidget::OnChoice1Clicked()
{
	if (unit)
	unit->ApplyMutation(C1);

	UE_LOG(LogTemp, Warning, TEXT("Choice 1 clicked"));
}

void UMutationWidget::OnChoice2Clicked()
{
	if (unit)
	unit->ApplyMutation(C2);

	UE_LOG(LogTemp, Warning, TEXT("Choice 2 clicked"));
}

void UMutationWidget::OnChoice3Clicked()
{
	if (unit)
	unit->ApplyMutation(C3);

	UE_LOG(LogTemp, Warning, TEXT("Choice 3 clicked"));
}
