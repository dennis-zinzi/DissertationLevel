// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../AI/EnemyCharacter.h"
#include "../DissertationLevelCharacter.h"

/**
 * All based on Pseudocode from: www.kfish.org/boids/pseudocode.html
 */
class DISSERTATIONLEVEL_API BoidFlock
{
	public:
		BoidFlock(TArray<AEnemyCharacter*> AIs, ADissertationLevelCharacter *Player);
		~BoidFlock();

		void UpdateAIPositions();

		FORCEINLINE TArray<AEnemyCharacter*> GetAllAIs(){
			return AIs;
		}
    
        FORCEINLINE ADissertationLevelCharacter* GetPlayer(){
            return Player;
        }

	private:
		TArray<AEnemyCharacter*> AIs;
        ADissertationLevelCharacter *Player;
    

		FVector CalculateBoidCohesion(AEnemyCharacter *AI);
		FVector CalculateBoidAlignment(AEnemyCharacter *AI);
		FVector CalculateBoidSeparation(AEnemyCharacter *AI);
		void LimitVelocity(AEnemyCharacter *AI);
};
