// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "DissertationLevel.h"
#include "DissertationLevelGameMode.h"
#include "DissertationLevelCharacter.h"
#include "AI/EnemyAIController.h"
#include "AI/EnemyCharacter.h"
#include "Game/PlayerTrail.h"

ADissertationLevelGameMode::ADissertationLevelGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	
	if(PlayerPawnBPClass.Class != NULL){
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}


void ADissertationLevelGameMode::BeginPlay(){
	Super::BeginPlay();

	CurrentState = EPlayState::EPlaying;


}


void ADissertationLevelGameMode::Tick(float DeltaTime){
	Super::Tick(DeltaTime);

	//if(((int)DeltaTime % 6000) == 0){
	//	ADissertationLevelCharacter *Player = Cast<ADissertationLevelCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

	//	//Check for player, and game still active
	//	if(Player && CurrentState == EPlayState::EPlaying){
	//		FVector pos = Player->GetActorLocation() - FVector(75.0f, 75.0f, 75.0f);
	//		FRotator rotation(0.0f, 0.0f, 0.0f);
	//		FActorSpawnParameters SpawnInfo;

	//		//Generate TargetPoint at player pos (player trail)
	//		GetWorld()->SpawnActor<APlayerTrail>(pos, rotation, SpawnInfo);
	//	}
	//}
}


void ADissertationLevelGameMode::HandleNewState(EPlayState state){
	switch(state){
		//If game is playing
		case EPlayState::EPlaying:
			//spawn volumes active
			break;

		//If game is won
		case EPlayState::EWin:
		{
			//Get the AI controller
			TArray<AActor*> AIs;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyAIController::StaticClass(), AIs);
			AEnemyAIController *ai = Cast<AEnemyAIController>(AIs[0]);

			if(ai){
				//Stop AI from moving anymore
				ai->StopMovement();
				//Kill behavior tree
				ai->StopBehavior();

				AEnemyCharacter *AIChar = Cast<AEnemyCharacter>(ai->GetPawn());

				if(AIChar){
					//Ragdoll the AI
					AIChar->GetMesh()->SetSimulatePhysics(true);
					AIChar->GetMovementComponent()->MovementState.bCanJump = false;
				}

				//Unpossess the AI to stop behavior tree tasks
				ai->UnPossess();
			}

			
			break;
		}
		//If game is lost
		case EPlayState::EGameOver:
			//block player input
			APlayerController *playerController = UGameplayStatics::GetPlayerController(this, 0);
			if(playerController){
				playerController->SetCinematicMode(true, false, false, true, true);
			}

			//ragdoll the character
			ACharacter *myCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
			if(myCharacter){
				myCharacter->GetMesh()->SetSimulatePhysics(true);
				myCharacter->GetMovementComponent()->MovementState.bCanJump = false;
			}

			//Get the AI controller
			TArray<AActor*> AIs;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyAIController::StaticClass(), AIs);
			AEnemyAIController *ai = Cast<AEnemyAIController>(AIs[0]);

			if(ai){
				//Stop AI from moving anymore
				ai->StopMovement();
				//Kill behavior tree
				ai->StopBehavior();
			}

			break;
			//Unknown/default state
			/*default:
			case EBatteryPlayState::EUnknown:
			break;*/
	}
}