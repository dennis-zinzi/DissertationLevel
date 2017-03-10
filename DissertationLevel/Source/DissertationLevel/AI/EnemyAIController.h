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

	UPROPERTY(EditAnywhere, Category = "GridMap")
	float PointsDistance = 80.0f;

	UPROPERTY(EditAnywhere, Category = "GridMap")
	float RangeDistance = 800.0f;


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

		//Stops BehaviorTree Execution
		void StopBehavior();



		FORCEINLINE void AddToNodesList(PathNode &pn){
			Nodes.Add(&pn);
		}

		FORCEINLINE TArray<PathNode*> GetNodesList(){
			return Nodes;
		}

		FORCEINLINE void ClearLists(){
			Nodes.Empty();
			OpenList.Empty();
			ClosedList.Empty();
		}

		//Get connected/neighboring nodes for each node
		void FindEveryNeighborNodes();

		//Get A* Vector list of locations to move through
		TArray<FVector> GetAStarPath(APawn *Pawn);

		void ChasePlayer(APawn *Pawn);

		//Create GridMap
		void CreateGridMap(APawn *Pawn);

	private:
		//Blackboard keys
		uint8 EnemyKeyID;
		uint8 PatrolPointKeyID;

		//Patrol Points Array
		TArray<AActor*> PatrolPoints;

		//Current Patrol Point
		int32 CurrentPatrolPoint;

		//AStar Nodes List
		TArray<PathNode*> Nodes;

		//AStar Open List
		TArray<PathNode*> OpenList;

		//AStar Closed List
		TArray<PathNode*> ClosedList;

		bool AStarAlgorithm(const PathNode &StartNode, const PathNode &FinalNode);
		int HeuristicCost(const FVector &Source, const FVector &Destination);
		int CostToMove(const FVector &Source, const FVector &Destination);

		void Initialization(const PathNode &StartNode, const PathNode &FinalNode);
		TArray<FVector> GeneratePath(const PathNode &StartNode, const PathNode &FinalNode);

		PathNode* GetMinCostNode();
		PathNode* GetMatchingNode(const int ID, const TArray<PathNode*> &List);
		PathNode* GetNodeAtPos(const FVector &Pos, const TArray<PathNode*> &List);
		PathNode* GetNodeWithXY(const float x, const float y, const TArray<PathNode*> &List);
		bool FindNodeConnections(PathNode *Node);
};
