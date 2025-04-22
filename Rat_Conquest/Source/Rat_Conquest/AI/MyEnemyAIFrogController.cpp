// Fill out your copyright notice in the Description page of Project Settings.


#include "MyEnemyAIFrogController.h"
#include "Rat_Conquest/Unit/GeneralAIUnit.h"
#include "Rat_Conquest/Unit/PlayerUnit.h"
#include "Rat_Conquest/Managers/GameManager/GameManager.h"
#include "Rat_Conquest/Managers/GridManager/GridManager.h"
#include "Rat_Conquest/GridTile/GridTile.h"

void AMyEnemyAIFrogController::StartBodySlam()
{
	APlayerUnit* AI = Cast<AGeneralAIUnit>(GetPawn());
	if (AI)
	{
		AI->animationToPlay = FVector2D(75, 0); 
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AMyEnemyAIFrogController::BodySlam, 1.2f, false);
	}
}

void AMyEnemyAIFrogController::BodySlam()
{
	//Play VFX

	TArray<TObjectPtr<APlayerUnit>> PlayerUnitsInRange;
	AGeneralAIUnit* AI = Cast<AGeneralAIUnit>(GetPawn());
	if (AI)
	{
		AI->SpawnVFX();

		auto PlayerUnits = AI->GameManager->PlayerUnits; 
		//check distance is within 2 tiles
		for (auto PlayerUnit : PlayerUnits)
		{
			if (IsValid(PlayerUnit.Get()))
			{
				float Distance = AI->ChebyshevDistance(AI->CurrentGridPosition, PlayerUnit->CurrentGridPosition);
				if (Distance <= 2)
				{
					PlayerUnitsInRange.Add(PlayerUnit);
				}
			}
		}
	}

	for (auto PlayerUnit : PlayerUnitsInRange)
	{
		if (!IsValid(PlayerUnit.Get()) || !IsValid(AI)) continue;

		FVector2D PlayerPos = PlayerUnit->CurrentGridPosition;
		FVector2D AIPos = AI->CurrentGridPosition;


		FVector2D Direction = PlayerPos - AIPos;
		Direction.Normalize();

		FVector2D KnockbackTile = PlayerPos + Direction; 

		auto Tile = AI->GridManager->GetTileAtPosition(KnockbackTile.X, KnockbackTile.Y);
		if (!Tile.IsValid())
		{
			continue; 
		}
		if (Tile->bIsOccupied && Tile->unitRefrence.IsValid())
		{
			continue;
		}

		PlayerUnit->TakeDamageFromEnemy(int(AI->Damage/2));

		FKnockbackLerpData LerpData;
		LerpData.Unit = PlayerUnit;
		LerpData.StartLocation = PlayerUnit->GetActorLocation();
		LerpData.TargetLocation = Tile->GetActorLocation();
		PlayerUnit->SetNewPosition(Tile->GridPosition);

		LerpData.Alpha = 0.0f;

		KnockbackUnits.Add(LerpData);
	}
	AI->FinishTurn();
}

void AMyEnemyAIFrogController::ToungeGrab()
{
	AGeneralAIUnit* AI = Cast<AGeneralAIUnit>(GetPawn());
	if (!AI || !Target.IsValid()) return;

	// Find a tile in front of AI
	for (auto tile : AI->GridManager->GetNeighbourTiles(AI->CurrentGridPosition.X, AI->CurrentGridPosition.Y))
	{
		if (tile.IsValid() && !tile->bIsOccupied && !tile->unitRefrence.IsValid())
		{
			AI->ToungeState = true;
			AI->SetToungeWorldTarget(Target->GetActorLocation());

			//Get bone location
			BoneLocation = AI->GetMesh()->GetBoneLocation("Bone_Tongue");
			FTonguePullData PullData;
			PullData.Unit = Target;
			PullData.StartLocation = Target->GetActorLocation();
			PullData.TargetLocation = tile->GetActorLocation();
			PullData.TargetGridPosition = tile->GridPosition;
			PullData.Alpha = 0.0f;
			PullData.BoneTargetPtr = &AI->TongueWorldTarget;

			TonguePullUnits.Add(PullData);
			break;
		}
	}
}

void AMyEnemyAIFrogController::ChooseAction()
{
	this->Target = nullptr;
	switch (Difficulty)
	{
	case EEnemyAIDifficulty::Easy:
		this->Target = FindClosestEnemy();
		break;
	case EEnemyAIDifficulty::Normal:
		this->Target = FindEnemyByThreat();
		break;
	case EEnemyAIDifficulty::Hard:
		this->Target = FindMostVulnerableEnemy();
		break;
	}
	//Log the target
	if (this->Target.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Target found: %s"), *this->Target->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No target found"));
		return;
	}
	AGeneralAIUnit* AI = Cast<AGeneralAIUnit>(GetPawn());
	if (!AI) return;
	if (AI->ChebyshevDistance(AI->CurrentGridPosition, Target->CurrentGridPosition) >= AI->MovementSpeed && FMath::RandRange(0, 1) == 0)
	{
		ToungeGrab();
		return;
	}
	//25% chance to use body slam
	if (FMath::RandRange(0, 3) == 0)
	{
		StartBodySlam();
		return;
	}
	else
	{
		MeleeAttack();
	}

}

void AMyEnemyAIFrogController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Knockback logic
	for (int32 i = KnockbackUnits.Num() - 1; i >= 0; --i)
	{
		FKnockbackLerpData& Data = KnockbackUnits[i];

		if (!Data.Unit.IsValid())
		{
			KnockbackUnits.RemoveAt(i);
			continue;
		}

		Data.Alpha += DeltaTime / KnockbackDuration;
		FVector LerpXZ = FMath::Lerp(
			FVector(Data.StartLocation.X, 0.f, Data.StartLocation.Z),
			FVector(Data.TargetLocation.X, 0.f, Data.TargetLocation.Z),
			Data.Alpha
		);

		float FixedY = Data.StartLocation.Y;

		FVector NewLocation = FVector(LerpXZ.X, FixedY, LerpXZ.Z);
		Data.Unit->SetActorLocation(NewLocation);

		if (Data.Alpha >= 1.0f)
		{
			Data.Unit->SetActorLocation(Data.TargetLocation);
			KnockbackUnits.RemoveAt(i);
		}
	}

	// Tongue pull logic
	for (int32 i = TonguePullUnits.Num() - 1; i >= 0; --i)
	{
		FTonguePullData& Data = TonguePullUnits[i];

		if (!Data.Unit.IsValid())
		{
			TonguePullUnits.RemoveAt(i);
			continue;
		}

		Data.Alpha += DeltaTime / KnockbackDuration;
		FVector NewLocation = FMath::Lerp(Data.StartLocation, Data.TargetLocation, Data.Alpha);
		Data.Unit->SetActorLocation(NewLocation);

		if (AGeneralAIUnit* AI = Cast<AGeneralAIUnit>(GetPawn()))
		{
			AI->SetToungeWorldTarget(NewLocation);
		}

		if (Data.Alpha >= 1.0f)
		{
			Data.Unit->SetActorLocation(Data.TargetLocation);
			if (Data.Unit->GridManager.IsValid())
			{
				FVector2D GridPos = Data.Unit->GridManager->WorldToGridPosition(Data.TargetLocation);
				Data.Unit->CurrentGridPosition = GridPos;
				Data.Unit->SetNewPosition(Data.TargetGridPosition);
			}

			TonguePullUnits.RemoveAt(i);
			AGeneralAIUnit* AI = Cast<AGeneralAIUnit>(GetPawn());
			AI->ToungeState = false;
			AI->SetToungeWorldTarget(BoneLocation);

			AI->FinishTurn();
		}
	}
}


