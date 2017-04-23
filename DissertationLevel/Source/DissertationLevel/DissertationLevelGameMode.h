// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"

#include "Boids/BoidFlock.h"
#include "Game/WinningLocation.h"
#include "AI/PathNode.h"

#include <time.h>
#include <stdio.h>

#include "DissertationLevelGameMode.generated.h"

UENUM(BlueprintType)
enum class EPlayState{
	EPlaying,
	EGameOver,
	EWin,
	EUnknown
};

UCLASS(minimalapi)
class ADissertationLevelGameMode : public AGameMode
{
	GENERATED_BODY()

	public:
		ADissertationLevelGameMode();

		virtual void BeginPlay() override;

		virtual void Tick(float DeltaTime) override;
		
		//Current playing state getter
		UFUNCTION(BlueprintPure, Category = "PlayState")
		inline EPlayState GetCurrentState() const{
			return CurrentState;
		}

		//Current playing state setter
		//UFUNCTION(BlueprintPure, Category = "PlayState")
		inline void SetCurrentState(EPlayState state){
			CurrentState = state;
			//handle the new state
			HandleNewState(state);
		}
    
        FORCEINLINE AWinningLocation* GetWinLoc(){
            return WinLoc;
        }
		
		FORCEINLINE TArray<PathNode*> GetMapNodes(){
			return MapNodes;
		}
    
        FORCEINLINE BoidFlock* GetAIFlock(){
            return AIFlock;
        }

	private:
		//Flock of AI Characters
		BoidFlock *AIFlock;
    
        //Winning Location
        AWinningLocation *WinLoc;

		//Current Game State
		EPlayState CurrentState;
		//Change Game State function
		void HandleNewState(EPlayState state);

		//List of Nodes in Map
		TArray<PathNode*> MapNodes;
    
        time_t StartTime;
};



