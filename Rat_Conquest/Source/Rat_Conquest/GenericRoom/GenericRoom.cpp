// Fill out your copyright notice in the Description page of Project Settings.


#include "GenericRoom/GenericRoom.h"

// Sets default values
AGenericRoom::AGenericRoom()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGenericRoom::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGenericRoom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

