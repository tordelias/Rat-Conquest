// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Blueprint/UserWidget.h"
#include "Engine/LocalPlayer.h"


void AMyPlayerController::SetupInputComponent()
{
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		// add the mapping context so we get controls
		Subsystem->AddMappingContext(InputMappingContext, 0);
	}
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	//Set up input component
	//SetupInputComponent();

	//create mouse melee attack pointer
	if (MouseMeleeAttackPointerClass)
	{
		MouseMeleeAttackPointer = CreateWidget<UUserWidget>(this, MouseMeleeAttackPointerClass);
		if (MouseMeleeAttackPointer)
		{
		}
	}
}

void AMyPlayerController::UseMouseMeleeAttackPointer()
{
	//Set mouse cursor to widget
	if (MouseMeleeAttackPointer)
	{
		//Set mouse cursor to widget
		SetMouseCursorWidget(EMouseCursor::Default, MouseMeleeAttackPointer);
	}
	UE_LOG(LogTemp, Error, TEXT("Called function in PC"));
}

void AMyPlayerController::changeMouseRotation(float rotation)
{
	if (MouseMeleeAttackPointer)
	{
		if (MouseMeleeAttackPointer)
		{
			UWidget* RootWidget = MouseMeleeAttackPointer->GetRootWidget();
			if (RootWidget)
			{
				RootWidget->SetRenderTransform(FWidgetTransform(FVector2D(0, 0), FVector2D(1, 1), FVector2D(0, 0), rotation));
				RootWidget->SynchronizeProperties();
			}
		}
	}
}



void AMyPlayerController::UseMouseDefaultPointer()
{
	// Set mouse cursor back to default
	SetMouseCursorWidget(EMouseCursor::Default, nullptr);
}
