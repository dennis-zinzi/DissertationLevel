// Fill out your copyright notice in the Description page of Project Settings.

#include "../DissertationLevel.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "EnemyCharacter.h"
#include "EnemyPatrolPoint.h"
#include "../DissertationLevelCharacter.h"
#include "../DissertationLevelGameMode.h"

#include "EnemyAIController.h"

#include <thread>
#include <chrono>


AEnemyAIController::AEnemyAIController(){
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));
	BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));

	CurrentPatrolPoint = 0;
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
		PatrolPointKeyID = BlackboardComp->GetKeyID("LocationToGo");

		//Populate patrol point array
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyPatrolPoint::StaticClass(), PatrolPoints);

		BehaviorComp->StartTree(*Char->AIBehavior);
	}
}


void AEnemyAIController::SetPlayerCaught(APawn *Pawn){
	if(!bIsPathing && BlackboardComp){
		BlackboardComp->SetValue<UBlackboardKeyType_Object>(EnemyKeyID, Pawn);
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, "YOU'VE BEEN SPOTTED!");

		//Restart to run EQS Query
		BehaviorComp->RestartTree();
		//BehaviorComp->StopTree();
	}
	
}


void AEnemyAIController::StopBehavior(){
	BehaviorComp->StopTree();
}


void AEnemyAIController::FindEveryNeighborNodes(){
	const float PointsDistance = 60.0f;

	for(int i = 0; i < Nodes.Num(); i++){
		PathNode *Curr = Nodes[i];

		for(float x = Curr->Position.X - PointsDistance; x < Curr->Position.X + PointsDistance + 1; x += PointsDistance){
			for(float y = Curr->Position.Y - PointsDistance; y < Curr->Position.Y + PointsDistance + 1; y += PointsDistance){
				PathNode* found = GetNodeWithXY(x, y, Nodes);//GetNodeAtPos(FVector(x, y, Curr->Position.Z), Nodes);

				if(found && found != nullptr){
					Curr->Connected.push_back(found->ID);
				}
			}
		}
		
		//if(Curr->Connected.Num() == 0){
		if(Curr->Connected.empty()){
			UE_LOG(LogClass, Log, TEXT("NO CONNECTIONS TO NODE with ID: %s"), *FString::FromInt(Curr->ID));
			//Remove node?
			Nodes.Remove(Curr);
		}
	}

}


bool AEnemyAIController::AStarAlgorithm(PathNode *StartNode, PathNode *FinalNode){
	if(Nodes.Num() == 0){
		UE_LOG(LogClass, Log, TEXT("NO NODES TO EXPLORE"));
		return false;
	}
	
	//Initialization
	float G = CostToMove(StartNode->Position, StartNode->Position),
		H = HeuristicCost(StartNode->Position, FinalNode->Position);

	StartNode->Cost = G + H;
	OpenList.Add(StartNode);

	//End Initialization

	if(OpenList.Num() == 0){
		UE_LOG(LogClass, Log, TEXT("NOTHING IN OPEN LIST"));
		return false;
	}
	
	PathNode *P = GetMinCostNode();

	if(!P || P == nullptr){
		//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Purple, "NO NODES ON OPEN LIST");
		UE_LOG(LogClass, Log, TEXT("NODE NOT FOUND"));
		return false;
	}

	//Safety number to prevent infinite execution
	int num = 0;

	//While potential nodes exist 
	while(/*num < 50000 &&*/ OpenList.Num() > 0){
		if(P->ID == FinalNode->ID){
			UE_LOG(LogClass, Log, TEXT("WE HAVE A PATH"));
			return true;
		}

		//if(P->Connected.Num() == 0){
		if(P->Connected.empty()){
			//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Purple, "NO CONNECTION TO NODE");
			bool isConnected = FindNodeConnections(P);

			if(!isConnected){
				UE_LOG(LogClass, Log, TEXT("NO CONNECTIONS TO NODE, ID: %s; made %s calls"), *FString::FromInt(P->ID), *FString::FromInt(num));
				return false;
				//OpenList.Remove(P);
				//P = GetMinCostNode();
				//continue;
			}
		}
		

		for(int i = 0; i < P->Connected.size()/*.Num()*/; i++){
			PathNode *Q = GetMatchingNode(P->Connected[i], Nodes);
			//Q->Cost = HeuristicCost(Q->Position, FinalNode.Position) + CostToMove(P->Position, Q->Position);

			if(!Q || Q == nullptr){
				UE_LOG(LogClass, Log, TEXT("404 NODE with ID: %s NOT FOUND"), *FString::FromInt(P->Connected[i]));
				continue;
			}
			if(ClosedList.Contains(Q)){
				//UE_LOG(LogClass, Log, TEXT("NODE %s ALREADY IN CLOSED LIST"), *FString::FromInt(Q->ID));
				continue;
			}

			int cost = HeuristicCost(Q->Position, FinalNode->Position) + CostToMove(P->Position, Q->Position);

			if(OpenList.Contains(Q) && cost >= Q->Cost){
				continue;
			}
			else if(OpenList.Contains(Q) && cost < Q->Cost){
				Q->Cost = cost;
				Q->Parent = P;
			}
			else{
				Q->Cost = cost;
				Q->Parent = P;

				OpenList.Add(Q);
			}
			num++;
		}

		OpenList.Remove(P);
		ClosedList.Add(P);
		
		P = GetMinCostNode();
		num++;
	}

	//If reached here, no Path exists
	UE_LOG(LogClass, Log, TEXT("NO PATH MADE, %s made"), *FString::FromInt(num));
	return false;
}


int AEnemyAIController::HeuristicCost(const FVector &Source, const FVector &Destination){
	FVector dist = Source - Destination;
	return (int)(FMath::Abs(dist.X) + FMath::Abs(dist.Y) + FMath::Abs(dist.Z));
}


int AEnemyAIController::CostToMove(const FVector &Source, const FVector &Destination){
	FVector dist = Source - Destination;
	return (int)(FMath::Abs(dist.X) + FMath::Abs(dist.Y) + FMath::Abs(dist.Z));
}


TArray<FVector> AEnemyAIController::GeneratePath(PathNode *StartNode, PathNode *FinalNode){
	//Final path to Player
	TArray<FVector> Path;

	PathNode *R = FinalNode;

	while(R->ID != StartNode->ID){
		Path.Add(R->Position);
		R = R->Parent;
	}

	//Final position to add is players
	Path.Add(StartNode->Position);

	return Path;
}


PathNode* AEnemyAIController::GetMinCostNode(){
	PathNode *Min = OpenList.Num() > 0 ? OpenList[0] : nullptr;

	for(int i = 0; i < OpenList.Num(); i++){
		if(OpenList[i]->Cost < Min->Cost){
			Min = OpenList[i];
		}
	}

	return Min;
}


TArray<FVector> AEnemyAIController::GetAStarPath(const FVector &AIPos, const FVector &PlayerPos){
	//Get start node (AI)
	PathNode *start = GetNodeWithXY(AIPos.X, AIPos.Y, Nodes);//Nodes[0];//Nodes[Nodes.Num() - 2];//GetNodeAtPos(AIPos, Nodes);
	//Get start node (Player)
	PathNode *end = GetNodeWithXY(PlayerPos.X, PlayerPos.Y, Nodes);//Nodes[Nodes.Num() - 10];//GetNodeAtPos(PlayerPos, Nodes);

	//Check Nodes actually exist
	if(!start || start == nullptr){
		UE_LOG(LogClass, Log, TEXT("ERROR: Start node not found"));
		return TArray<FVector>();
	}
	if(!end || end == nullptr){
		UE_LOG(LogClass, Log, TEXT("ERROR: End node not found"));
		return TArray<FVector>();
	}

	//Check Path available
	if(AStarAlgorithm(start, end)){
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, "SUCCESS!!!");
		UE_LOG(LogClass, Log, TEXT("WE MADE IT?"));
		return GeneratePath(start, end);;
	}
	else{
		UE_LOG(LogClass, Log, TEXT("ERROR MAKING PATH"));
		return TArray<FVector>();
	}
}


void AEnemyAIController::ChasePlayer(APawn *Pawn){
	UE_LOG(LogClass, Log, TEXT("Attempting to Start A*"));
	BehaviorComp->StopTree();

	ADissertationLevelCharacter *Player = Cast<ADissertationLevelCharacter>(Pawn);
	FVector PlayerPos = Player->GetActorLocation(),
		AIPos = GetPawn()->GetActorLocation();

	if(Player){
		//bIsPathing = true;
		CreateGridMap(AIPos, GetPawn()->GetActorForwardVector(), PlayerPos);

		TArray<FVector> Locations = GetAStarPath(AIPos, PlayerPos);
		
		if(Locations.Num() > 0){
			UE_LOG(LogClass, Log, TEXT("ATTEMPTING TO GENERATE PATH!"));

			for(int i = 0; i < Locations.Num(); i++){
				//while(FVector::Dist(AIPos, Locations[i]) > 80.0f){
					GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Purple, "MOVING TOWARDS YOU!");
					MoveToLocation(Locations[i], 25.0f, true, true, true, true, 0, false);
					//std::this_thread::sleep_for(std::chrono::seconds(2));
				//}bash
			}

			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, "DONE MOVING BEYATCH!");
			UE_LOG(LogClass, Log, TEXT("PATH CREATED"));
		}
		else{
			UE_LOG(LogClass, Log, TEXT("PATH NOT FOUND"));
		}
	}

	//BehaviorComp->RestartTree();

	//bIsPathing = false;
}


/**
 * Creates PathNodes representing search space, populating Nodes Array
 */
void AEnemyAIController::CreateGridMap(const FVector &AIPos, const FVector &AIForwardVec, const FVector &PlayerPos){
	//Clear current nodes
	ClearLists();

	//Rotate Forward Vector by 90 degrees
	FVector LineVector = AIForwardVec.RotateAngleAxis(-90.0f, FVector(0.0f, 0.0f, 1.0f));

	//PathNode ID
	int ID = 0;

	//X Pos loop
	for(float x = AIPos.X - RangeDistance; x < AIPos.X + RangeDistance + 1; x += PointsDistance){
		//Y Pos loop
		for(float y = AIPos.Y; y < AIPos.Y + RangeDistance + 1; y += PointsDistance){
			//Generate Point at these coordinates
			FVector GridLoc = FVector(x, y, AIPos.Z);

			//Add node to list
			PathNode *pn = new PathNode(ID, GridLoc);
			Nodes.Add(pn);

			ID++;
		}
	}

	////Add Start location
	PathNode *start = new PathNode(ID + 1, AIPos);
	Nodes.Add(start);

	//Add End location
	PathNode *end = new PathNode(ID + 2, PlayerPos);
	Nodes.Add(end);
}


/**
 * Populates Neighboring nodes list for given Node
 */
bool AEnemyAIController::FindNodeConnections(PathNode *Node){
	//Empty list currently there
	Node->Connected.clear();/*.Empty();*/

	float NodeX = Node->Position.X,
		NodeY = Node->Position.Y;
	

	for(float x = NodeX - PointsDistance; x < NodeX + PointsDistance + 1; x += PointsDistance){
		for(float y = NodeY - PointsDistance; y < NodeY + PointsDistance + 1; y += PointsDistance){
			//Skip if location is current node's
			if(x == NodeX && y == NodeY){
				continue;
			}

			PathNode* found = GetNodeWithXY(x, y, Nodes);

			if(found && found != nullptr){
				Node->Connected.push_back(found->ID);
			}
		}
	}
	UE_LOG(LogClass, Log, TEXT("%s CONNECTIONS to ID: %s"), *FString::FromInt(Node->Connected.size()),*FString::FromInt(Node->ID));
	//UE_LOG(LogClass, Log, TEXT("CONNECTIONS to ID: %s %s"), *FString::FromInt(Node->Connected.size()), Node->Connected.empty() ? *FString("DONT EXIST") : *FString("EXIST"));

	return Node->Connected.empty()/*Num() > 0*/ ? false : true;
}


/*  ---------------------------------------------------------- 
 *	-----------------Node Helper Functions--------------------
 *	---------------------------------------------------------- */
PathNode* AEnemyAIController::GetMatchingNode(const int ID, const TArray<PathNode*> &List){
	for(int i = 0; i < List.Num(); i++){
		if(List[i]->ID == ID){
			return List[i];
		}
	}

	return nullptr;
}


PathNode* AEnemyAIController::GetNodeAtPos(const FVector &Pos, const TArray<PathNode*> &List){
	for(int i = 0; i < List.Num(); i++){
		if(List[i]->Position == Pos){
			return List[i];
		}
	}

	return nullptr;
}

PathNode* AEnemyAIController::GetNodeWithXY(const float x, const float y, const TArray<PathNode*> &List){
	for(int i = 0; i < List.Num(); i++){
		if(List[i]->Position.X == x && List[i]->Position.Y == y){
			return List[i];
		}
	}

	return nullptr;
}