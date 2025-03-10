// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

struct FInputActionValue;
class UInputMappingContext;
class UInputAction;
class UUserWidget;
/**
 * 
 */
UCLASS()
class RAT_CONQUEST_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

protected: 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* InputMappingContext;

	virtual void SetupInputComponent() override;

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> MouseMeleeAttackPointerClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UUserWidget> MouseMeleeAttackPointer;

public:
	void UseMouseMeleeAttackPointer();
	void changeMouseRotation(float rotation);

	void UseMouseDefaultPointer(); 
	
};
