// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnemyAIController.generated.h"

USTRUCT()
struct FNodeCost{
	GENERATED_BODY()

	FVector position;
	int cost;
	TArray<FNodeCost> connected;
	FNodeCost *parent;
};

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


	public:
		AEnemyAIController();
		virtual void Possess(APawn *InPawn) override;

		void SetPlayerCaught(APawn *Pawn);

		FORCEINLINE uint8 GetEnemyKeyID() const{
			return EnemyKeyID;
		}

		FORCEINLINE uint8 GetPatrolPointKeyID() const{
			return PatrolPointKeyID;
		}

		FORCEINLINE TArray<AActor*> GetPatrolPoints() const{
			return PatrolPoints;
		}

		FORCEINLINE int32 GetCurrentPatrolPoint() const{
			return CurrentPatrolPoint;
		}

		FORCEINLINE void SetCurrentPatrolPoint(int32 point){
			CurrentPatrolPoint = point;
		}

		FORCEINLINE UBlackboardComponent* GetBlackboardComp() const{
			return BlackboardComp;
		}


		void StopBehavior();


		FORCEINLINE void AddToLocations(FVector pos) {
			LocationsToGo.Add(pos);
		}

		void PopulateOpenList(TArray<FVector> &Locations);

	private:
		//Blackboard keys
		uint8 EnemyKeyID;
		uint8 PatrolPointKeyID;

		//Patrol Points Array
		TArray<AActor*> PatrolPoints;

		//Current Patrol Point
		int32 CurrentPatrolPoint;

		//AStar Nodes
		TArray<FVector> LocationsToGo;

		//AStar Open List
		TArray<FNodeCost> OpenList;

		//AStar Closed List
		TArray<FVector> ClosedList;

		bool AStarAlgorithm(TArray<FVector> &Locations, TArray<FNodeCost*> &OpenList, TArray<FNodeCost*> &ClosedList, const FVector &CurrentLoc, const FVector &Destination);
		int HeuristicCost(const FVector &Source, const FVector &Destination);
		int CostToMove(const FVector &Source, const FVector &Destination);

		void Initialization(TArray<FVector> &Locations, TArray<FNodeCost*> &OpenList, TArray<FNodeCost*> &ClosedList, const FVector &CurrentLoc, const FVector &Destination);
		TArray<FVector> GeneratePath(TArray<FNodeCost*> &ClosedList, const FVector &CurrentLoc, const FVector &Destination);

		FNodeCost GetMinCostNode(const TArray<FNodeCost*> &List);
};
