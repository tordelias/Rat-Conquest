// Fill out your copyright notice in the Description page of Project Settings.


#include "SelectRoomWidget.h"
#include "Rat_Conquest/Map/LevelGenerator.h"
#include "Components/Button.h"
void USelectRoomWidget::NativeConstruct()
{
	Super::NativeConstruct();
	UE_LOG(LogTemp, Warning, TEXT("RoomWidgett Constructed!"));  // ADD THIS LOG

	if (NorthButton) {

		NorthButton->OnClicked.AddDynamic(this, &USelectRoomWidget::OnNorthButtonClicked);
		ButtonList.Add(NorthButton);
	}
	if (EastButton) {

		EastButton->OnClicked.AddDynamic(this, &USelectRoomWidget::OnEastButtonClicked);
		ButtonList.Add(EastButton);
	}
	if (SouthButton) {

		SouthButton->OnClicked.AddDynamic(this, &USelectRoomWidget::OnSouthButtonClicked);
		ButtonList.Add(SouthButton);

	}
	if (WestButton) {

		WestButton->OnClicked.AddDynamic(this, &USelectRoomWidget::OnWestButtonClicked);
		ButtonList.Add(WestButton);

	}
	WestButton->SetVisibility(ESlateVisibility::Hidden);

}

void USelectRoomWidget::SetLevelGeneratorRefrence(ALevelGenerator* _LevelGenerator)
{
	LevelGen = _LevelGenerator;
	/*if (!_LevelGenerator || !LevelGen) {
		UE_LOG(LogTemp, Error, TEXT("LEVEL GENERATOR IS NULL!"));
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("LEVEL GENERATOR IS SET!"));
	}*/
}

void USelectRoomWidget::HideInaccessibleDoorButtons(TArray<bool> _DoorDirections)
{
	for (int i = 0; i < _DoorDirections.Num(); ++i) {

		if (!_DoorDirections[i]) {
			ButtonList[i]->SetVisibility(ESlateVisibility::Hidden);
		}
		else {
			ButtonList[i]->SetVisibility(ESlateVisibility::Visible);
		}

	}
}

void USelectRoomWidget::OnNorthButtonClicked()
{
	UE_LOG(LogTemp, Error, TEXT("NORTH BUTTON CLICKED"));
	LevelGen->MoveToRoom(0);
}

void USelectRoomWidget::OnEastButtonClicked()
{
	UE_LOG(LogTemp, Error, TEXT("EAST BUTTON CLICKED"));
	LevelGen->MoveToRoom(1);
}

void USelectRoomWidget::OnSouthButtonClicked()
{
	UE_LOG(LogTemp, Error, TEXT("SOUTH BUTTON CLICKED"));
	LevelGen->MoveToRoom(2);
}

void USelectRoomWidget::OnWestButtonClicked()
{
	UE_LOG(LogTemp, Error, TEXT("WEST BUTTON CLICKED"));
	LevelGen->MoveToRoom(3);
}
