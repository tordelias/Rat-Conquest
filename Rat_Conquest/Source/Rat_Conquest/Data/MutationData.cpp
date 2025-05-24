// Fill out your copyright notice in the Description page of Project Settings.


#include "MutationData.h"

MutationData::MutationData()
{
	speed = 0;
	attack = 0;
	defense = 0;
	health = 0;
	damage = 0;
	range = 0;
	initiative = 0;
	maxExperience = 10;
	maxChange = 5;
	minChange = 1;
	level = 0;
	experience = 0;
	stats.Add(speed);
	stats.Add(attack);
	stats.Add(defense);
	stats.Add(health);
	stats.Add(damage);
	stats.Add(range);
	stats.Add(initiative);

	//speed = 0, damage = 1, defense = 2, health = 3, attack = 4, range = 5, initiative = 6
}

MutationData::~MutationData()
{
}

void MutationData::Mutate()
{
	if (experience >= maxExperience)
	{
		while (statsC1 == statsC2 || statsC1 == statsC3 || statsC2 == statsC3)
		{
			statsC1 = MutateArray();
			statsC2 = MutateArray();
			statsC3 = MutateArray();
		}

		level++;
		experience = 0;
		maxExperience *= 2;
	}
}

TArray<int> MutationData::MutateArray()
{
	//speed = 0, damage = 1, defense = 2, health = 3, attack = 4, range = 5, initiative = 6
	TArray<int> mutatedArray = stats;

	int positiveChange = FMath::RandRange(minChange, maxChange);
	int negativeChange = FMath::RandRange(-maxChange, -minChange); 

	int stat1 = FMath::RandRange(0, stats.Num() - 1);
	int stat2 = GetNegativeStat(stat1); 

	mutatedArray[stat1] += positiveChange;
	mutatedArray[stat2] += negativeChange;

	return mutatedArray;
}

int MutationData::GetNegativeStat(int positiveStat)
{
	//speed = 0, damage = 1, defense = 2, health = 3, attack = 4, range = 5, initiative = 6
	TArray<int> NegativeStats;

	switch (positiveStat)
	{
	case 0: // Speed
		NegativeStats = { 1, 3 }; break; // Damage, Health
	case 1: // Damage
		NegativeStats = { 2, 3 }; break; // Defense, Health
	case 2: // Defense
		NegativeStats = { 0, 6 }; break; // Speed, Initiative
	case 3: // Health
		NegativeStats = { 0 }; break; // Speed
	case 4: // Attack
		NegativeStats = { 2 }; break; // Defense
	case 5: // Range
		NegativeStats = { 4, 6 }; break; // Attack, Initiative
	case 6: // Initiative
		NegativeStats = { 2, 3 }; break; // Defense, Health
	default:
		NegativeStats = { FMath::RandRange(0, stats.Num() - 1) };
		break;
	}

	return NegativeStats[FMath::RandRange(0, NegativeStats.Num() - 1)];
}

