#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Rat_Conquest/Unit/PlayerUnit.h"
#include "MutationWidget.generated.h"

/**
 *
 */
UCLASS()
class RAT_CONQUEST_API UMutationWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	// Called when the widget is constructed
	virtual void NativeConstruct() override;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Unit Stat Widget")
	class UTextBlock* UnitName;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Unit Stat Widget")
	class UTextBlock* HealthC1;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Unit Stat Widget")
	class UTextBlock* DamageC1;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Unit Stat Widget")
	class UTextBlock* SpeedC1;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Unit Stat Widget")
	class UTextBlock* DefenseC1;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Unit Stat Widget")
	class UTextBlock* HealthC2;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Unit Stat Widget")
	class UTextBlock* DamageC2;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Unit Stat Widget")
	class UTextBlock* SpeedC2;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Unit Stat Widget")
	class UTextBlock* DefenseC2;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Unit Stat Widget")
	class UTextBlock* HealthC3;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Unit Stat Widget")
	class UTextBlock* DamageC3;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Unit Stat Widget")
	class UTextBlock* SpeedC3;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Unit Stat Widget")
	class UTextBlock* DefenseC3;

	// Buttons
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Unit Stat Widget")
	class UButton* Choice1;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Unit Stat Widget")
	class UButton* Choice2;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Unit Stat Widget")
	class UButton* Choice3;

	UPROPERTY()
	APlayerUnit* unit;


	TArray<int> C1;
	TArray<int> C2;
	TArray<int> C3;

public:
	void SetData(TArray<int> statsC1, TArray<int> statsC2, TArray<int> statsC3, FName NewUnitName, APlayerUnit* currentUnit);

protected:
	// Button click event handlers
	UFUNCTION()
	void OnChoice1Clicked();

	UFUNCTION()
	void OnChoice2Clicked();

	UFUNCTION()
	void OnChoice3Clicked();
};
