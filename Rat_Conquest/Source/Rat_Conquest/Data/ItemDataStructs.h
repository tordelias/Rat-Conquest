#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemDataStructs.generated.h"

UENUM()
enum class EItemQuality : uint8
{
	Trash UMETA(DisplayName = "Trash"),
	Common UMETA(DisplayName = "Common"),
	Uncommon UMETA(DisplayName = "Uncommon"),
	Rare UMETA(DisplayName = "Rare"),
	SuperRare UMETA(DisplayName = "SuperRare"),
	Epic UMETA(DisplayName = "Epic"),
	Legendary UMETA(DisplayName = "Legendary"),
	Mythic UMETA(DisplayName = "Mythic"),
	DemiGod UMETA(DisplayName = "DemiGod"),
	God UMETA(DisplayName = "God")
};

UENUM()
enum class EItemType : uint8
{
	Armor UMETA(DisplayName = "Armor"),
	Weapon UMETA(DisplayName = "Weapon"),
	Shield UMETA(DisplayName = "Shield"),
	Spell UMETA(DisplayName = "Spell"),
	Artifact UMETA(DisplayName = "Artifact"),
	Consumable UMETA(DisplayName = "Consumable")
};

USTRUCT()
struct FItemStatistics
{
	GENERATED_USTRUCT_BODY()


	UPROPERTY(EditAnywhere)
	float Defence;
	
	UPROPERTY(EditAnywhere)
	float Damage;
	UPROPERTY(EditAnywhere)
	float Attack;

	UPROPERTY(EditAnywhere)
	float Range;

	UPROPERTY(EditAnywhere)
	float MovementSpeed;

	UPROPERTY(EditAnywhere)
	float MaxHealth;

	UPROPERTY(EditAnywhere)
	float Luck;

	UPROPERTY(EditAnywhere)
	float Initiative;



	UPROPERTY(EditAnywhere)
	bool bIsMelee; 


	UPROPERTY(EditAnywhere)
	float SellValue;
};

USTRUCT()
struct FItemTextData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	FText Name;

	UPROPERTY(EditAnywhere)
	FText Description;

	UPROPERTY(EditAnywhere)
	FText IteractionText;

	UPROPERTY(EditAnywhere)
	FText UsageText;
};

USTRUCT()
struct FItemAssetData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere)
	UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere)
	UTexture2D* UnitIcon;

	//blendspace for animations
	UPROPERTY(EditAnywhere)
	UBlendSpace* BlendSpace;
};

USTRUCT()
struct FItemData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()


	UPROPERTY(EditAnywhere, Category = "ItemData")
	FName ID; 

	UPROPERTY(EditAnywhere, Category = "ItemData")
	EItemType ItemType;

	UPROPERTY(EditAnywhere, Category = "ItemData")
	EItemQuality ItemQuality;

	UPROPERTY(EditAnywhere, Category = "ItemData")
	FItemStatistics ItemStatistics;

	UPROPERTY(EditAnywhere, Category = "ItemData")
	FItemTextData ItemTextData;

	UPROPERTY(EditAnywhere, Category = "ItemData")
	FItemAssetData ItemAssetData;


};
