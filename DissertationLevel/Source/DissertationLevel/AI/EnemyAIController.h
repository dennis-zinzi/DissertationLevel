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

		FORCEINLINE TArray<FVector> GetLocationsToGo(){
			return LocationsToGo;
		}

		FORCEINLINE void AddToOpenList(PathNode &pn){
			Nodes.Add(&pn);
		}

		void PopulateOpenList();

		int GetVectorID(const FVector &pos) const;

		TArray<FVector> GetAStarPath();

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

		//AStar Nodes List
		TArray<PathNode*> Nodes;

		//AStar Open List
		TArray<PathNode*> OpenList;

		//AStar Closed List
		TArray<PathNode*> ClosedList;

		bool AStarAlgorithm(const PathNode &CurrentNode, const PathNode &FinalNode);
		int HeuristicCost(const FVector &Source, const FVector &Destination);
		int CostToMove(const FVector &Source, const FVector &Destination);

		void Initialization(const PathNode &CurrentNode, const PathNode &FinalNode);
		TArray<FVector> GeneratePath(const PathNode &CurrentNode, const PathNode &FinalNode);

		PathNode* GetMinCostNode(const TArray<PathNode*> &List);
		PathNode* GetMatchingNode(const int ID, const TArray<PathNode*> &List);
};
