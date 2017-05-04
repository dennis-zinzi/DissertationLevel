// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../AI/EnemyCharacter.h"
#include "../AI/PathNode.h"
#include "../Game/WinningLocation.h"
#include "../Game/WorldObject.h"

/**
 * All based on Pseudocode from: www.kfish.org/boids/pseudocode.html
 */
class DISSERTATIONLEVEL_API BoidFlock
{
	public:
		BoidFlock(TArray<AEnemyCharacter*> AIs, AWinningLocation *WinLoc, TArray<PathNode*> MapNodes);
		~BoidFlock();

		void UpdateAIPositions();
        FVector UpdateAI(AEnemyCharacter *AI, const FVector &PosToGo);

		FORCEINLINE TArray<AEnemyCharacter*> GetAllAIs(){
			return AIs;
		}
    
        void CalculateBoidsPaths();
        FVector GetAICenter();

	private:
		TArray<AEnemyCharacter*> AIs;
        AWinningLocation *WinLoc;
    
        TArray<PathNode*> MapNodes;
    

		FVector CalculateBoidCohesion(AEnemyCharacter *AI);
		FVector CalculateBoidAlignment(AEnemyCharacter *AI);
		FVector CalculateBoidSeparation(AEnemyCharacter *AI);
		FVector CalculateGoalTendency(AEnemyCharacter *AI, const FVector &PosToGo);
        void LimitVelocity(AEnemyCharacter *AI);
};
