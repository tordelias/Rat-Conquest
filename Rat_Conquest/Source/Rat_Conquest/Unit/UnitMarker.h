// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UnitMarker.generated.h"

UCLASS()
class RAT_CONQUEST_API AUnitMarker : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* UnitMarkerMesh;
	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* SceneRoot;
public:	
	// Sets default values for this actor's properties
	AUnitMarker();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	TWeakObjectPtr<AActor> TargetActor;

	void SetTargetActor(AActor* NewTargetActor)
	{
		TargetActor = NewTargetActor;
	}

	void SetPosition(FVector NewPosition);

};
