// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Room.generated.h"



UCLASS()
class RAT_CONQUEST_API ARoom : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARoom();
	UFUNCTION(BlueprintCallable)
	void SetGridPosition(FVector2D Position) { GridPosition = Position; }

	UFUNCTION(BlueprintPure)
	FVector2D GetGridPosition() const { return GridPosition; }

	UFUNCTION(BlueprintCallable)
	bool& GetDoorDirection(int32 dirIndex);

	TArray<bool> GetDoorDirections(TArray<bool>);

	UFUNCTION(BlueprintCallable, Category = "Room")
	int32 GetDoorCount() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Room")
	FVector2D RoomSize = FVector2D(1, 1);

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* RoomMesh;
	void DrawDebugDoors();

	UFUNCTION(BlueprintCallable)
	void ConstructBridges();

public:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables",
		Meta = (Tooltip = "Door directions: 0=North, 1=East, 2=South, 3=West"))
	TArray<bool> DoorDirections;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	bool bIsBossRoom = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	bool bIsExplored = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	bool bIsLootRoom = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	int bridgeLength = 10;

	UPROPERTY(EditAnywhere, Category = "Grid Settings")
	TSubclassOf<AActor> GridTileClass;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
private:
	FVector2D GridPosition;
};
