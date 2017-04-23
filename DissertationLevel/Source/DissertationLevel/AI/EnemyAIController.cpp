// Fill out your copyright notice in the Description page of Project Settings.

#include "../DissertationLevel.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "EnemyCharacter.h"
#include "EnemyPatrolPoint.h"
#include "../DissertationLevelCharacter.h"
#include "../DissertationLevelGameMode.h"
#include "AStar.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Navigation/CrowdManager.h"

#include "EnemyAIController.h"

#include <vector>
#include <algorithm>

using std::vector;
using std::reverse;

#define MAX_ITERATIONS 10000
#define MOVE_ITERATIONS 500


AEnemyAIController::AEnemyAIController(const FObjectInitializer &ObjInitializer)
: Super(ObjInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComp"))){
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));
	BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));
//    PathfollowingComp = CreateDefaultSubobject<UCrowdFollowingComponent>(TEXT("PathFollowingComp"));
//    
//    PathfollowingComp->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Good);
//    UCrowdManager::

	CurrentPatrolPoint = 0;
    CurrentPathLocIndex = 0;
	bIsPathing = false;
}

void AEnemyAIController::Possess(APawn *InPawn){
	Super::Possess(InPawn);

	AEnemyCharacter *Char = Cast<AEnemyCharacter>(InPawn);

	if(Char && Char->AIBehavior){
		BlackboardComp->InitializeBlackboard(*Char->AIBehavior->BlackboardAsset);

		//Get Player KeyID from Blackboard
		EnemyKeyID = BlackboardComp->GetKeyID("Target");

		//Get Patrol Point KeyID from Blackboard
		PatrolPointKeyID = BlackboardComp->GetKeyID("NextPatrolPoint");
        
        //Get Path Location KeyID from Blackboard
        PathLocationKeyID = BlackboardComp->GetKeyID("NextPathLocation");

		//Populate patrol point array
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyPatrolPoint::StaticClass(), PatrolPoints);

		BehaviorComp->StartTree(*Char->AIBehavior);
	}
}


void AEnemyAIController::SetPlayerCaught(APawn *Pawn){
	if(!bIsPathing && BlackboardComp){
		BlackboardComp->SetValue<UBlackboardKeyType_Object>(EnemyKeyID, Pawn);
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, "YOU'VE BEEN SPOTTED!");

		//Restart to run MoveToPlayerTask immediately
		BehaviorComp->RestartTree();
	}
	
}


void AEnemyAIController::StopBehavior(){
	BehaviorComp->StopTree();
}


/*
 *  ----------------------------------------------------------
 *	----------------A* Pathfinding Functions------------------
 *	----------------------------------------------------------
 */

/**
 * Called when player withing AI's catching distance
 */
void AEnemyAIController::ChasePlayer(APawn *Pawn, TArray<PathNode*> &MapNodes){
	UE_LOG(LogClass, Log, TEXT("Attempting to Start A*"));
	BehaviorComp->StopTree();

	ADissertationLevelCharacter *Player = Cast<ADissertationLevelCharacter>(Pawn);


	if(Player){
        FVector PlayerPos = Player->GetActorLocation(),
            AIPos = GetPawn()->GetActorLocation();
        
        //Create the A* path
        TArray<FVector> Locations = AStar::GetAStarPath(AIPos, PlayerPos, MapNodes);
		
		if(Locations.Num() > 0){
			UE_LOG(LogClass, Log, TEXT("ATTEMPTING TO GENERATE PATH!"));

            //Move AI along the A* path
            for(auto Loc : Locations){
                MoveToLocation(Loc, 25.0f, true, true, true, true, 0, false);
            }

			UE_LOG(LogClass, Log, TEXT("PATH CREATED"));
		}
		else{
			UE_LOG(LogClass, Log, TEXT("PATH NOT FOUND"));
		}
	}

	BehaviorComp->RestartTree();
}


/**
 * Called when player withing AI's catching distance
 */
void AEnemyAIController::GoToWinningLocation(AActor *WinLoc, TArray<PathNode*> &MapNodes){
    //if(!bIsPathing){
    UE_LOG(LogClass, Log, TEXT("Attempting to Start A*"));
    BehaviorComp->StopTree();
    
    AWinningLocation *WinningLoc = Cast<AWinningLocation>(WinLoc);
    
    if(WinningLoc){
        FVector WinningPos = WinningLoc->GetActorLocation(),
            AIPos = GetPawn()->GetActorLocation();
        
        UE_LOG(LogClass, Log, TEXT("ATTEMPTING TO GENERATE PATH!"));
        
        //Create the A* path
        TArray<FVector> Locations = AStar::GetAStarPath(AIPos, WinningPos, MapNodes);//GetAStarPath(AIPos, WinningPos);
        
        if(Locations.Num() > 0){
            //Set path
            PathLocations = Locations;
            
            UE_LOG(LogClass, Log, TEXT("PATH CREATED"));
            bIsPathing = true;
        }
//        else if(Locations.Num() > 0 && Locations.Num() <= 2){
//            //Set Game to Won
//            ((ADissertationLevelGameMode *)GetWorld()->GetAuthGameMode())->SetCurrentState(EPlayState::EWin);
//        }
        else{
            UE_LOG(LogClass, Log, TEXT("PATH NOT FOUND"));
        }
    }
    
    BehaviorComp->RestartTree();
    //}
}


/**
 * Adds new location to path (used for crowd)
 */
void AEnemyAIController::AddPathLocation(const FVector &Location, TArray<PathNode*> &MapNodes){
    PathNode *PN = AStar::GetClosestNode(Location, MapNodes);
    
    PathLocations.Add(PN->Position);
}







