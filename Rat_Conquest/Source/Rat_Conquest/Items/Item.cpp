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

void AItem::EquipItem(AActor* UnitOwner)
{
	UE_LOG(LogTemp, Warning, TEXT("Item Equipped"));
    if (!UnitOwner)
    {
        UE_LOG(LogTemp, Error, TEXT("UnitOwner is null. Cannot equip item."));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("Item Equipped"));

    // Make the item a child of the unit that picked it up
    AttachToActor(UnitOwner, FAttachmentTransformRules::KeepRelativeTransform);

    // Position the item relative to the unit
    FVector Offset(0.f, 0.f, 80.f); // Adjust the offset as needed
    SetActorRelativeLocation(Offset);
	ItemMesh->SetVisibility(false);
    bIsEquipped = true;    
}

void AItem::DropItem()
{
	UE_LOG(LogTemp, Warning, TEXT("Item Dropped"));
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	ItemMesh->SetVisibility(true);
    bIsEquipped = false;
}

void AItem::UseItem()
{
	UE_LOG(LogTemp, Warning, TEXT("Item Used"));
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
        bIsMelee = ItemBase->ItemStatistics.bIsMelee;
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

void AItem::InitializeAbilities()
{

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

    //rotate the item

    if (!bIsEquipped) {
        FRotator Rotation = FRotator(0.f, 1.f * YawRotationSpeed, 0.f);
        AddActorLocalRotation(Rotation);

        // Add a bobbing effect to the item
        float RunningTime = GetGameTimeSinceCreation();
        float DeltaHeight = (FMath::Sin((RunningTime + DeltaTime) * BobbingSpeed) - FMath::Sin(RunningTime * BobbingSpeed));
        FVector NewLocation = GetActorLocation();
        NewLocation.Z += DeltaHeight * 8.0f;
        RunningTime += DeltaTime;
        SetActorLocation(NewLocation);


    }
	



}

