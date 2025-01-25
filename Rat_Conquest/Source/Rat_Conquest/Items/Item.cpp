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
	ItemMesh->SetVisibility(true);
}

void AItem::ConsumeItem()
{
	UE_LOG(LogTemp, Warning, TEXT("Item Consumed"));
}

void AItem::InitializeItem()
{
    if (!ItemDataTable) return;

   
    FItemData* ItemData = ItemDataTable->FindRow<FItemData>(RowName, TEXT(""));
    for (const auto& RowNames : ItemDataTable->GetRowNames())
    {
        UE_LOG(LogTemp, Warning, TEXT("Row Name: %s"), *RowNames.ToString());
    }
    if (ItemData)
    {
        UItemBase* ItemBase = NewObject<UItemBase>();
        ItemBase->ID = ItemData->ID;
        ItemBase->ItemType = ItemData->ItemType;
        ItemBase->ItemQuality = ItemData->ItemQuality;
        ItemBase->ItemStatistics = ItemData->ItemStatistics;
        ItemBase->ItemTextData = ItemData->ItemTextData;
        ItemBase->ItemAssetData = ItemData->ItemAssetData;

        Damage = ItemBase->ItemStatistics.Damage;
        if (ItemBase->ItemAssetData.Mesh)
        {
            ItemMesh->SetStaticMesh(ItemBase->ItemAssetData.Mesh);
            UE_LOG(LogTemp, Warning, TEXT("Static Mesh Set Successfully: %s"), *ItemBase->ItemAssetData.Mesh->GetName());
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Static Mesh is null for Item ID: %s"), *ItemBase->ID.ToString());
        }

	
        // Assign the item to this actor or elsewhere
    }
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	InitializeItem();

}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

