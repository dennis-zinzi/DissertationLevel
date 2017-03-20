// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"

#include "Boids/BoidFlock.h"
#include "Game/WinningLocation.h"

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
    

	private:
		//Flock of AI Characters
		BoidFlock *AIFlock;
    
        //Winning Location
        AWinningLocation *WinLoc;

		EPlayState CurrentState;

		void HandleNewState(EPlayState state);

};



