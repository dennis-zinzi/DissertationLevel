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

#define MAX_ITERATIONS 10000


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
 * Actual A* Algorithm
 */
bool AEnemyAIController::AStarAlgorithm(PathNode *StartNode, PathNode *FinalNode){
    //Check there are actually nodes to traverse
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
	
    //Safety number to prevent infinite execution
    int num = 0;
    
    //While nodes to explore are available create path
    while(OpenList.Num() > 0 && num < MAX_ITERATIONS){
        PathNode *Current = GetMinCostNode();
        
        //Check node is real
        if(!Current || Current == nullptr){
            UE_LOG(LogClass, Log, TEXT("NODE NOT FOUND"));
            return false;
        }
        
        //Remove current node from Open List (already explored)
        OpenList.Remove(Current);
        
        //Check if Node has children, if not find them
        if(Current->Connected.empty()){
            bool isConnected = FindNodeConnections(Current);
            
            //Check node actually has connections, reject it if it doesn't
            if(!isConnected){
                UE_LOG(LogClass, Log, TEXT("NO CONNECTIONS TO NODE, ID: %s"), *FString::FromInt(Current->ID));
                continue;
            }
        }
        
        //Iterate through all of the connected nodes
        for(int i = 0; i < Current->Connected.size(); i++){
            //Get Child node
            PathNode *Successor = GetMatchingNode(Current->Connected[i], Nodes);
            if(!Successor || Successor == nullptr){
                continue;
            }
            
            //If the child node is the goal, we have a winner
            if(Successor->ID == FinalNode->ID){
                UE_LOG(LogClass, Log, TEXT("WE HAVE A PATH"));
                return true;
            }
            
            //Recalculate Cost to reach node
            const int cost = CostToMove(Current->Position, Successor->Position) + HeuristicCost(Successor->Position, FinalNode->Position);
            
            //If Node already in open list with a cheaper cost, then ignore
            if(OpenList.Contains(Successor) && Successor->Cost < cost){
                continue;
            }
            //If Node already in closed list with a cheaper cost, then ignore
            if(ClosedList.Contains(Successor) && Successor->Cost < cost){
                continue;
            }
            
            //Otherwise update the child's cost, and set it's parent to current node
            Successor->Cost = cost;
            Successor->Parent = Current;
            
            //If child not in open list, add it
            if(!OpenList.Contains(Successor)){
                OpenList.Add(Successor);
            }
            num++;
        }
        
        //Add Current node to closed list
        ClosedList.Add(Current);
        num++;
    }
    
    //If reached here, no path exists to player
    UE_LOG(LogClass, Log, TEXT("NO POSSIBLE PATH, after %s iterations"), *FString::FromInt(num));
    return false;
}


/**
 * Determines Heuristic cost of moving from one position to another, using Manhattan Distance
 */
int AEnemyAIController::HeuristicCost(const FVector &Source, const FVector &Destination){
	FVector dist = Source - Destination;
	return (int)(FMath::Abs(dist.X) + FMath::Abs(dist.Y) + FMath::Abs(dist.Z));
}


/**
 * Determines cost of moving from one position to another, using Manhattan Distance
 */
int AEnemyAIController::CostToMove(const FVector &Source, const FVector &Destination){
	FVector dist = Source - Destination;
	return (int)(FMath::Abs(dist.X) + FMath::Abs(dist.Y) + FMath::Abs(dist.Z));
}


/**
 * Generates the FVector Array of positions to in the path
 */
TArray<FVector> AEnemyAIController::GeneratePath(PathNode *StartNode, PathNode *FinalNode){
	//Final path to Player
	TArray<FVector> Path;
    
    ClosedList.Sort([](const PathNode &node1, const PathNode &node2){
        return node1.ID > node2.ID;
    });
    
    //Final position to add is players
    ClosedList.Add(FinalNode);
    
    for(auto Node : ClosedList){
        Path.Add(Node->Position);
    }
    
	return Path;
    
    //	PathNode *R = FinalNode;
    //
    //	while(R->ID != StartNode->ID){
    //		Path.Add(R->Position);
    //		R = R->Parent;
    //	}
    //
    //	//Final position to add is players
    //	Path.Add(StartNode->Position);
}


/**
 * Prepares and calls A* algorithm
 */
TArray<FVector> AEnemyAIController::GetAStarPath(const FVector &AIPos, const FVector &PlayerPos){
	//Get start node (AI)
    PathNode *start = GetClosestNode(AIPos, Nodes);
	//Get start node (Player)
    PathNode *end = GetClosestNode(PlayerPos, Nodes);

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
		UE_LOG(LogClass, Log, TEXT("PATH IS AVAILABLE!!!"));
		return GeneratePath(start, end);;
	}
	else{
		UE_LOG(LogClass, Log, TEXT("ERROR MAKING PATH"));
		return TArray<FVector>();
	}
}


/**
 * Called when player withing AI's catching distance
 */
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
                GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Purple, "MOVING TOWARDS YOU!");
                
                int num = 0;
				while(FVector::Dist(AIPos, Locations[i]) > 80.0f && num < 1000){
					MoveToLocation(Locations[i], 25.0f, true, true, true, true, 0, false);
                    num++;
				}
			}

			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, "DONE MOVING BEYATCH!");
			UE_LOG(LogClass, Log, TEXT("PATH CREATED"));
		}
		else{
			UE_LOG(LogClass, Log, TEXT("PATH NOT FOUND"));
		}
	}

	BehaviorComp->RestartTree();
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
		for(float y = AIPos.Y - RangeDistance; y < AIPos.Y + RangeDistance + 1; y += PointsDistance){
			//Generate Point at these coordinates
			FVector GridLoc = FVector(x, y, AIPos.Z);

			//Add node to list
			PathNode *pn = new PathNode(ID, GridLoc);
			Nodes.Add(pn);

			ID++;
		}
	}
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


/*  
 *  ----------------------------------------------------------
 *	---------------PathNode Helper Functions------------------
 *	---------------------------------------------------------- 
 */
PathNode* AEnemyAIController::GetMatchingNode(const int ID, const TArray<PathNode*> &List){
	for(int i = 0; i < List.Num(); i++){
		if(List[i]->ID == ID){
			return List[i];
		}
	}

	return nullptr;
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


PathNode* AEnemyAIController::GetNodeWithXY(const float x, const float y, const TArray<PathNode*> &List){
	for(int i = 0; i < List.Num(); i++){
		if(List[i]->Position.X == x && List[i]->Position.Y == y){
			return List[i];
		}
	}

	return nullptr;
}


PathNode* AEnemyAIController::GetClosestNode(const FVector &Pos, const TArray<PathNode*> &List){
    PathNode *Closest = List.Num() > 0 ? List[0] : nullptr;
    float dist = 999999.0f;
    
    for(int i = 0; i < List.Num(); i++){
        float DistToNode = FMath::Abs(FVector::Dist(Pos, List[i]->Position));
        
        if(DistToNode < dist){
            Closest = List[i];
            dist = DistToNode;
        }
    }
    
    return Closest;
}

