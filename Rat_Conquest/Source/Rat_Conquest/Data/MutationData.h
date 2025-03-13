// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class RAT_CONQUEST_API MutationData
{
private: 
	int experience;
	int level;
	int maxExperience; // Experience needed to level up
	int maxChange; // Maximum change in stats
	int minChange; // Minimum change in stats

	int speed;
	int attack;
	int defense;
	int health;
	int damage;
	int range;
	int initiative;
	TArray<int> stats;

	TArray<int> statsC1;
	TArray<int> statsC2;
	TArray<int> statsC3;
public:
	MutationData();
	~MutationData();

	void Mutate(); 
	TArray<int> MutateArray();

	void SetExperienceNeeded(int experienceNeeded) { maxExperience = experienceNeeded; };
	void AddExperience(int experienceToAdd) { experience += experienceToAdd; };
	int GetExperience() const { return experience; };
	int GetLevel() const { return level; };
	int GetMaxExperience() const { return maxExperience; };
	void SetMaxChange(int max) { maxChange = max; };
	void SetMinChange(int min) { minChange = min; };
	TArray<int> GetStatsC1() const { return statsC1; };
	TArray<int> GetStatsC2() const { return statsC2; };
	TArray<int> GetStatsC3() const { return statsC3; };

	
};
