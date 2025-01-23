// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	RootComponent = ItemMesh;
}

void AItem::EquipItem()
{
	UE_LOG(LogTemp, Warning, TEXT("Item Equipped"));
	ItemMesh->SetVisibility(false);

}

void AItem::DropItem()
{
	UE_LOG(LogTemp, Warning, TEXT("Item Dropped"));
	ItemMesh->SetVisibility(false);
}

void AItem::ConsumeItem()
{
	UE_LOG(LogTemp, Warning, TEXT("Item Consumed"));
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

