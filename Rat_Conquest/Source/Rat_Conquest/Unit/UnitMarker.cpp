// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitMarker.h"

// Sets default values
AUnitMarker::AUnitMarker()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;
	UnitMarkerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("UnitMarkerMesh"));
	UnitMarkerMesh->SetupAttachment(RootComponent);
	UnitMarkerMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	UnitMarkerMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	UnitMarkerMesh->SetRenderCustomDepth(true); 
	UnitMarkerMesh->SetCustomDepthStencilValue(2);
	//Set scale to 0,2
	UnitMarkerMesh->SetRelativeScale3D(FVector(0.2f, 0.2f, 0.2f));

	// Set the mesh for the marker
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MarkerMeshAsset(TEXT("StaticMesh'/Game/Assets/UnitMarker/UnitMarker.UnitMarker'"));

	if (MarkerMeshAsset.Succeeded())
	{
		UnitMarkerMesh->SetStaticMesh(MarkerMeshAsset.Object);
	}
	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load UnitMarker mesh!"));
	}

}

// Called when the game starts or when spawned
void AUnitMarker::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AUnitMarker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (TargetActor.IsValid())
	{
		FVector TargetLocation = TargetActor->GetActorLocation();
		SetPosition(TargetLocation);
	}
	else
	{
		SetPosition(FVector::ZeroVector);
	}

}

void AUnitMarker::SetPosition(FVector NewPosition)
{
	static FVector Offset(0.f, 0.f, 75.f); 
	NewPosition += Offset; 
	SetActorLocation(NewPosition);
}

