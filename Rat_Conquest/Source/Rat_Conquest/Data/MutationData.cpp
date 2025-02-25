// Fill out your copyright notice in the Description page of Project Settings.


#include "MutationData.h"

MutationData::MutationData()
{
	speed = 0;
	attack = 0;
	defense = 0;
	health = 0;
	maxExperience = 10;
	maxChange = 5;
	minChange = 1;
	level = 0;
	experience = 0;
	stats.Add(speed);
	stats.Add(attack);
	stats.Add(defense);
	stats.Add(health);

	//speed = 0, attack = 1, defense = 2, health = 3
}

MutationData::~MutationData()
{
}

void MutationData::Mutate()
{
	if (experience >= maxExperience)
	{
		statsC1 = MutateArray();
		statsC2 = MutateArray();
		statsC3 = MutateArray();

		level++;
		experience = 0;
		maxExperience *= 2;
	}
}

TArray<int> MutationData::MutateArray()
{
	//speed = 0, attack = 1, defense = 2, health = 3
	TArray<int> mutatedArray = stats; 

		int positiveChange = FMath::RandRange(minChange, maxChange);
		int negativeChange = FMath::RandRange(-minChange, -maxChange);

		int stat1 = FMath::RandRange(0, stats.Num() - 1);
		int stat2 = FMath::RandRange(0, stats.Num() - 1);
		while (stat1 == stat2)
		{
			stat2 = FMath::RandRange(0, stats.Num() - 1);
		}
		mutatedArray[stat1] += positiveChange;
		mutatedArray[stat2] += negativeChange;

		return mutatedArray;
}
