// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../AI/EnemyCharacter.h"

/**
 * All based on Pseudocode from: www.kfish.org/boids/pseudocode.html
 */
class DISSERTATIONLEVEL_API BoidFlock
{
	public:
		BoidFlock(TArray<AEnemyCharacter*> AIs);
		~BoidFlock();

		void UpdateAIPositions();

		FORCEINLINE TArray<AEnemyCharacter*> GetAllAIs(){
			return AIs;
		}

	private:
		TArray<AEnemyCharacter*> AIs;

		FVector CalculateBoidCohesion(AEnemyCharacter *AI);
		FVector CalculateBoidAlignment(AEnemyCharacter *AI);
		FVector CalculateBoidSeparation(AEnemyCharacter *AI);
		FVector LimitVelocity(AEnemyCharacter *AI);
};
