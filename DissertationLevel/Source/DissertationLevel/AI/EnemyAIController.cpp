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

#define IN_RANGE_DISTANCE 800.0f
#define CAUGHT_DISTANCE 80.0f

AEnemyAIController::AEnemyAIController(){
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));
	BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));

	CurrentPatrolPoint = 0;
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
	if(BlackboardComp){
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
					Curr->Connected.Add(found->ID);
				}
			}
		}
		
		if(Curr->Connected.Num() == 0){
			UE_LOG(LogClass, Log, TEXT("NO CONNECTIONS TO NODE with ID: %s"), *FString::FromInt(Curr->ID));
			//Remove node?
			Nodes.Remove(Curr);
		}
	}

}


bool AEnemyAIController::AStarAlgorithm(const PathNode &StartNode, const PathNode &FinalNode){
	if(Nodes.Num() == 0){
		UE_LOG(LogClass, Log, TEXT("NOTHING IN Nodes LIST"));
		return false;
	}
	
	Initialization(StartNode, FinalNode);

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
	int num = 0;
	while(OpenList.Num() > 0){
		if(P->ID == FinalNode.ID){
			UE_LOG(LogClass, Log, TEXT("WE HAVE A PATH"));
			return true;
		}

		if(P->Connected.Num() == 0){
			//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Purple, "NO CONNECTION TO NODE");
			bool isConnected = FindNodeConnections(P);

			if(!isConnected){
				UE_LOG(LogClass, Log, TEXT("NO CONNECTIONS TO NODE, ID: %s; made %s calls"), *FString::FromInt(P->ID), *FString::FromInt(num));
				return false;
			}
		}
		

		for(int i = 0; i < P->Connected.Num(); i++){
			PathNode *Q = GetMatchingNode(P->Connected[i], Nodes);
			//Q->Cost = HeuristicCost(Q->Position, FinalNode.Position) + CostToMove(P->Position, Q->Position);

			if(!Q || Q == nullptr){
				UE_LOG(LogClass, Log, TEXT("404 NODE with ID: %s NOT FOUND"), *FString::FromInt(P->Connected[i]));
				continue;
			}

			int cost = HeuristicCost(Q->Position, FinalNode.Position) + CostToMove(P->Position, Q->Position);

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


void AEnemyAIController::Initialization(const PathNode &StartNode, const PathNode &FinalNode) {
	float G = CostToMove(StartNode.Position, StartNode.Position),
		H = HeuristicCost(StartNode.Position, FinalNode.Position);

	PathNode start(StartNode.ID, StartNode.Position, G + H);
	OpenList.Add(&start);
}

TArray<FVector> AEnemyAIController::GeneratePath(const PathNode &StartNode, const PathNode &FinalNode){
	//Final path to Player
	TArray<FVector> Path;

	PathNode R = FinalNode;

	while(R.ID != StartNode.ID){
		Path.Add(R.Position);
		R = *R.Parent;
	}

	//Final position to add is players
	Path.Add(StartNode.Position);

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


TArray<FVector> AEnemyAIController::GetAStarPath(APawn *Pawn){
	FindEveryNeighborNodes();
	PathNode *start = Nodes[0];//GetNodeWithXY(GetPawn()->GetActorLocation().X, GetPawn()->GetActorLocation().Y, Nodes);//GetMatchingNode(Nodes[Nodes.Num() - 2]->ID, Nodes);//GetNodeAtPos(GetPawn()->GetActorLocation(), Nodes);
	PathNode *end = GetMatchingNode(Nodes[Nodes.Num() - 1]->ID, Nodes);//GetNodeAtPos(Pawn->GetActorLocation(), Nodes);

	if(!start || start == nullptr){
		UE_LOG(LogClass, Log, TEXT("ERROR: Start node not found"));
		return TArray<FVector>();
	}
	if(!end || end == nullptr){
		UE_LOG(LogClass, Log, TEXT("ERROR: End node not found"));
		return TArray<FVector>();
	}

	if(AStarAlgorithm(*start, *end)){
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, "SUCCESS!!!");
		UE_LOG(LogClass, Log, TEXT("WE MADE IT?"));
		return GeneratePath(*start, *end);;
	}
	else{
		UE_LOG(LogClass, Log, TEXT("ERROR MAKING PATH"));
		return TArray<FVector>();
	}
}


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

void AEnemyAIController::ChasePlayer(APawn *Pawn){
	UE_LOG(LogClass, Log, TEXT("Attempting to Start A*"));
	ADissertationLevelCharacter *Player = Cast<ADissertationLevelCharacter>(Pawn);

	if(Player){
		CreateGridMap(Player);

		TArray<FVector> Locations = GetAStarPath(Player);
		
		if(Locations.Num() > 0){
			UE_LOG(LogClass, Log, TEXT("ATTEMPTING TO GENERATE PATH!"));

			for(int i = 0; i < Locations.Num(); i++){
				MoveToLocation(Locations[i], 25.0f, true, true, true, true, 0, false);
			}

			UE_LOG(LogClass, Log, TEXT("PATH CREATED"));
		}
		else{
			UE_LOG(LogClass, Log, TEXT("PATH NOT FOUND"));
		}
	}
}


void AEnemyAIController::CreateGridMap(APawn *Pawn){
	//Clear current nodes
	ClearLists();

	//Store its location and its forward vector
	FVector PawnLocation = GetPawn()->GetActorLocation();
	//Rotate Vector by 90 degrees
	FVector LineVector = GetPawn()->GetActorForwardVector().RotateAngleAxis(-90.0f, FVector(0.0f, 0.0f, 1.0f));

	//PathNode ID
	int ID = 0;

	//X Pos loop
	for(float x = PawnLocation.X - RangeDistance; x < PawnLocation.X + RangeDistance + 1; x += PointsDistance){
		//Y Pos loop
		for(float y = PawnLocation.Y; y < PawnLocation.Y + RangeDistance + 1; y += PointsDistance){
			//Generate Point at these coordinates
			FVector GridLoc = FVector(x, y, PawnLocation.Z);

			//Add node to list
			PathNode *pn = new PathNode(ID, GridLoc);
			Nodes.Add(pn);

			ID++;
		}
	}

	////Add Start location
	//PathNode *start = new PathNode(ID + 1, PawnLocation);
	//Nodes.Add(start);

	//Add End location
	ADissertationLevelCharacter *Player = Cast<ADissertationLevelCharacter>(Pawn);
	PathNode *end = new PathNode(ID + 1, Player->GetActorLocation());
	Nodes.Add(end);
}


bool AEnemyAIController::FindNodeConnections(PathNode *Node){
	if(Node->Connected.Num() > 0){
		return true;
	}
	for(float x = Node->Position.X - PointsDistance; x < Node->Position.X + PointsDistance + 1; x += PointsDistance){
		for(float y = Node->Position.Y - PointsDistance; y < Node->Position.Y + PointsDistance + 1; y += PointsDistance){
			PathNode* found = GetNodeWithXY(x, y, Nodes);

			if(found && found != nullptr){
				Node->Connected.Add(found->ID);
			}
		}
	}

	return Node->Connected.Num() > 0 ? true : false;
}