// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SelectRoomWidget.generated.h"

/**
 * 
 */
class ALevelGenerator;
UCLASS()
class RAT_CONQUEST_API USelectRoomWidget : public UUserWidget
{
	GENERATED_BODY()


	virtual void NativeConstruct() override;
public:
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Room Widget")
	class UButton* NorthButton;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Room Widget")
	class UButton* EastButton;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Room Widget")
	class UButton* SouthButton;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Room Widget")
	class UButton* WestButton;

	UFUNCTION()
	void SetLevelGeneratorRefrence(ALevelGenerator* _LevelGenerator);

	UFUNCTION()
	void HideInaccessibleDoorButtons();

	UFUNCTION()
	void OnNorthButtonClicked();

	UFUNCTION()
	void OnEastButtonClicked();

	UFUNCTION()
	void OnSouthButtonClicked();

	UFUNCTION()
	void OnWestButtonClicked();

	ALevelGenerator* LevelGen;
};
