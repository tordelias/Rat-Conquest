// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthBar.h"
#include "Components/ProgressBar.h"


void UHealthBar::SetHealthBarPercentage(float Percentage)
{
	HealthBar->SetPercent(Percentage);
	// Hide widget if full health
	if (Percentage < 1.0f)
	{
		HealthBar->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		HealthBar->SetVisibility(ESlateVisibility::Collapsed);
	}
}
