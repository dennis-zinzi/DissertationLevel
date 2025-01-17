// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "PathNode.h"
#include "EnemyAIController.generated.h"


/**
 * 
 */
UCLASS()
class DISSERTATIONLEVEL_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
	UPROPERTY(Transient)
	UBlackboardComponent *BlackboardComp;

	UPROPERTY(Transient)
	class UBehaviorTreeComponent *BehaviorComp;
    
//    UPROPERTY()
//    class UCrowdFollowingComponent *PathfollowingComp;

	public:
		AEnemyAIController(const FObjectInitializer &ObjInitializer);
		virtual void Possess(APawn *InPawn) override;

		void SetPlayerCaught(APawn *Pawn);

        /* Blackboard key IDs */
		FORCEINLINE uint8 GetEnemyKeyID() const{
			return EnemyKeyID;
		}

		FORCEINLINE uint8 GetPatrolPointKeyID() const{
			return PatrolPointKeyID;
		}
    
        FORCEINLINE uint8 GetPathLocationKeyID() const{
            return PathLocationKeyID;
        }

    
        /* Patrolling getters & setters */
		FORCEINLINE TArray<AActor*> GetPatrolPoints() const{
			return PatrolPoints;
		}

		FORCEINLINE int32 GetCurrentPatrolPoint() const{
			return CurrentPatrolPoint;
		}

		FORCEINLINE void SetCurrentPatrolPoint(int32 point){
			CurrentPatrolPoint = point;
		}
    
    
        /* Pathfinding getters & setters */
        FORCEINLINE TArray<FVector> GetPathLocations() const{
            return PathLocations;
        }
    
        FORCEINLINE void SetPathLocations(const TArray<FVector> &PathLocs){
            PathLocations = PathLocs;
        }
    
        FORCEINLINE int GetCurrentPathLocIndex() const{
            return CurrentPathLocIndex;
        }
        
        FORCEINLINE void SetCurrentPathLocIndex(int index){
            CurrentPathLocIndex = index;
        }
    

        //Getter for Blackboard Component
		FORCEINLINE UBlackboardComponent* GetBlackboardComp() const{
			return BlackboardComp;
		}
    

		//Stops BehaviorTree Execution
		void StopBehavior();


		//Get A* Vector list of locations to move through
		void ChasePlayer(APawn *Pawn, TArray<PathNode*> &MapNodes);
        void GoToWinningLocation(AActor *WinLoc, TArray<PathNode*> &MapNodes);
    
        //Add new path location
        void AddPathLocation(const FVector &Location, TArray<PathNode*> &MapNodes);


	private:
		//Blackboard keys
		uint8 EnemyKeyID;
		uint8 PatrolPointKeyID;
        uint8 PathLocationKeyID;

		//Patrol Points Array
		TArray<AActor*> PatrolPoints;

		//Current Patrol Point
		int32 CurrentPatrolPoint;

        //Path Locations Array
        TArray<FVector> PathLocations;
    
        //Current Path Location index
        int CurrentPathLocIndex;
    
    

		bool bIsPathing;
};
