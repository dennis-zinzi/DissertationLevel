// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "DissertationLevel.h"
#include "DissertationLevelGameMode.h"
#include "DissertationLevelCharacter.h"
#include "AI/EnemyAIController.h"
#include "AI/EnemyCharacter.h"
#include "Game/WorldObject.h"
#include "AI/AStar.h"

#define WORLD_OBJECTS_TO_SPAWN 50
#define WORLD_POS_LIMIT 1400.0f


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

	//Initialize Play State to playing
	CurrentState = EPlayState::EPlaying;

	//Create Node Grid Map
    MapNodes = AStar::CreateGridMap();

	//Create random objects in the world
	for(int i = 0; i < WORLD_OBJECTS_TO_SPAWN; i++){
		float posX = FMath::RandRange(-WORLD_POS_LIMIT, WORLD_POS_LIMIT),
			posY = FMath::RandRange(-WORLD_POS_LIMIT, WORLD_POS_LIMIT);

		FVector loc = FVector(posX, posY, 125.0f);
		FRotator rotation(0.0f, 0.0f, 0.0f);
		FActorSpawnParameters SpawnInfo;

		//Generate TargetPoint at player pos (player trail)
		GetWorld()->SpawnActor<AWorldObject>(loc, rotation, SpawnInfo);
	}
    
    //Determine Unpassable locations (where world objects are)
    TArray<AActor*> WorldObjArr;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWorldObject::StaticClass(), WorldObjArr);
    
    for(auto WorldObj : WorldObjArr){
        AWorldObject *Obj = Cast<AWorldObject>(WorldObj);
        
        if(Obj){
            //Determine nodes overlapping
            AStar::CheckOverlappingNodes(Obj->GetComponentsBoundingBox().Min, Obj->GetComponentsBoundingBox().Max, MapNodes);
        }
    }

	//Get all the AIs
	TArray<AActor*> AllAIs;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyCharacter::StaticClass(), AllAIs);

	//Cast all the AIs to EnemyCharacters
	TArray<AEnemyCharacter*> AIChars;
	for(auto Actor : AllAIs){
		AEnemyCharacter *AIChar = Cast<AEnemyCharacter>(Actor);

		if(AIChar){
			AIChars.Add(AIChar);
		}
	}

    //Get Winning Location
    TArray<AActor*> WinLocArr;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWinningLocation::StaticClass(), WinLocArr);
    
    WinLoc = Cast<AWinningLocation>(WinLocArr[0]);
    
    if(WinLoc){
        AIFlock = new BoidFlock(AIChars, WinLoc, MapNodes);
    }

}


void ADissertationLevelGameMode::Tick(float DeltaTime){
	Super::Tick(DeltaTime);

	//COMMENT IF NOT USING FLOCK BEHAVIOR
//	if(((int)DeltaTime % 60000) == 0){
//		AIFlock->UpdateAIPositions();
//	}
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
			TArray<AActor*> AllAIs;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyAIController::StaticClass(), AllAIs);
			AEnemyAIController *ai = Cast<AEnemyAIController>(AllAIs[0]);

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

			////Get the AI controller
			//TArray<AActor*> AIs;
			//UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyAIController::StaticClass(), AIs);
			//AEnemyAIController *ai = Cast<AEnemyAIController>(AIs[0]);

			//if(ai){
			//	//Stop AI from moving anymore
			//	ai->StopMovement();
			//	//Kill behavior tree
			//	//ai->StopBehavior();
			//}

			break;
			//Unknown/default state
			/*default:
			case EPlayState::EUnknown:
			break;*/
	}
}

