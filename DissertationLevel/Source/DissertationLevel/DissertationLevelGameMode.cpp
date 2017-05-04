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

        FVector loc = AStar::GetClosestNode(FVector(posX, posY, 130.0f), MapNodes)->Position;//FVector(posX, posY, 130.0f);
        
		//Generate WorldObject at random position
		GetWorld()->SpawnActor<AWorldObject>(loc, FRotator::ZeroRotator, FActorSpawnParameters());
	}
    
    
//    //Generate WorldObject
//    GetWorld()->SpawnActor<AWorldObject>(AStar::GetClosestNode(FVector(50.0f, 0.0f, 130.0f), MapNodes)->Position, FRotator(10.0f, 0.0f, 0.0f), FActorSpawnParameters());
//    //Generate WorldObject
//    GetWorld()->SpawnActor<AWorldObject>(AStar::GetClosestNode(FVector(99.0f, 0.0f, 130.0f), MapNodes)->Position, FRotator(5.0f, 0.0f, 0.0f), FActorSpawnParameters());

    
    //Determine Unpassable locations (where world objects are)
    TArray<AActor*> WorldObjArr;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWorldObject::StaticClass(), WorldObjArr);
    
    for(auto WorldObj : WorldObjArr){
        AWorldObject *Obj = Cast<AWorldObject>(WorldObj);
        
        if(Obj){
            //Determine nodes overlapping with world objects
            AStar::CheckOverlappingNodes(Obj->GetComponentsBoundingBox().Min, Obj->GetComponentsBoundingBox().Max, MapNodes);
            
            //            UE_LOG(LogClass, Log, TEXT("Center : %s, Min: %s, Max: %s"), *Obj->GetActorLocation().ToString(), *Obj->GetComponentsBoundingBox().Min.ToString(), *Obj->GetComponentsBoundingBox().Max.ToString());

        }
    }
    
    //Get Winning Location
    TArray<AActor*> WinLocArr;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWinningLocation::StaticClass(), WinLocArr);
    
    if(Cast<AWinningLocation>(WinLocArr[0])){
        WinLoc = Cast<AWinningLocation>(WinLocArr[0]);
    }

    /* Comment if using single AI */
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


    AIFlock = new BoidFlock(AIChars, WinLoc, MapNodes);
    
    //Start profiling file
    GEngine->Exec(GetWorld(), TEXT("Stat StartFile"));
    
    //Start clock
    time(&StartTime);

}


void ADissertationLevelGameMode::Tick(float DeltaTime){
	Super::Tick(DeltaTime);
    
//    FVector pos = UGameplayStatics::GetPlayerController(this, 0)->GetPawn()->GetActorLocation();
//    UE_LOG(LogClass, Log, TEXT("POS: %s"), *pos.ToString());
    
    //Get all the AIs to check if all reached destination
    TArray<AActor*> AllAIs;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyCharacter::StaticClass(), AllAIs);

    int done = 0;
    
    //Cast all the AIs to EnemyCharacters
    for(auto Actor : AllAIs){
        AEnemyCharacter *AIChar = Cast<AEnemyCharacter>(Actor);

        if(AIChar){
            AEnemyAIController *Cont = Cast<AEnemyAIController>(AIChar->GetInstigatorController());
            if(Cont){
                if(Cont->GetBlackboardComp()->GetValueAsBool(FName("Reached"))){
                    done++;
                }
            }
        }
    }
    
    //If every AI has reached destination stop timer
    if(done == AllAIs.Num()){
        time_t EndTime;
        time(&EndTime);
        double ExecutionTime = difftime(EndTime, StartTime);
        
        FString method = AIFlock ? "crowd" : "individual";
        
        UE_LOG(LogClass, Log, TEXT("Took %s to reach Winloc for %s AIs (%s)"), *FString::SanitizeFloat(ExecutionTime),
               *FString::FromInt(AllAIs.Num()), *method);
        
        GEngine->Exec(GetWorld(), TEXT("Stat StopFile"));
        SetCurrentState(EPlayState::EWin);
        AIFlock = nullptr;
    }
    
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
            
            for(auto AI : AllAIs){
                AEnemyAIController *ai = Cast<AEnemyAIController>(AI);
                
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

