// Fill out your copyright notice in the Description page of Project Settings.

#include "../DissertationLevel.h"
#include "AStar.h"

#include <vector>
#include <algorithm>

using std::vector;
using std::reverse;

#define FLOOR_Z 130.0f
#define WORLD_MIN_X -1800.0f //-1760.0f
#define WORLD_MAX_X 1000.0f //960.0f
#define WORLD_MIN_Y -1400.0f //-1360.0f
#define WORLD_MAX_Y 1400.0f //1360.0f

#define NODE_DISTANCE 50.0f //80.0f
#define MAX_ITERATIONS 10000

AStar::AStar(){
}

AStar::~AStar(){
}


/**
 * Generates the Path Node Map necessary to generate a path through the environment
 */
TArray<PathNode*> AStar::CreateGridMap(){
    //Array to store Path Nodes in
    TArray<PathNode*> MapNodes;
    
    //PathNode ID
    int ID = 0;
    
    //X Pos loop
    for(float x = WORLD_MIN_X; x < WORLD_MAX_X + 1; x += NODE_DISTANCE){
        //Y Pos loop
        for(float y = WORLD_MIN_Y; y < WORLD_MAX_Y + 1; y += NODE_DISTANCE){
            //Generate Point at these coordinates
            FVector GridLoc = FVector(x, y, FLOOR_Z);
            
            //Add node to list
            PathNode *pn = new PathNode(ID, GridLoc);
            MapNodes.Add(pn);
            
            ID++;
        }
    }
    
    UE_LOG(LogClass, Log, TEXT("Nodes Created: %s"), *FString::FromInt(MapNodes.Num()));
    return MapNodes;
}


/**
 * Determines all nodes within given Vector bounds, and makes them umpassable in the list
 */
void AStar::CheckOverlappingNodes(const FVector &Start, const FVector &End, const TArray<PathNode*> &List){
//    int n = 0;
    for(auto Node : List){
        if(Start.X <= Node->Position.X && Node->Position.X <= End.X
           && Start.Y <= Node->Position.Y && Node->Position.Y <= End.Y){
            //If checks above are true, must be overlapping, so set isPassable flag to false
            Node->bIsPassable = false;
//            n++;
        }
    }
//    UE_LOG(LogClass, Log, TEXT("BLOCKED NODES, %s"), *FString::FromInt(n));
}


/**
 * Prepares and calls A* algorithm
 */
TArray<FVector> AStar::GetAStarPath(const FVector &StartPos, const FVector &EndPos, TArray<PathNode*> &NodeMap){
    //Get start node (AI)
    PathNode *start = GetClosestNode(StartPos, NodeMap);
    //Get start node (Player)
    PathNode *end = GetClosestNode(EndPos, NodeMap);
    
    //Check Nodes actually exist
    if(!start){
        UE_LOG(LogClass, Log, TEXT("ERROR: Start node not found"));
        return TArray<FVector>();
    }
    if(!end){
        UE_LOG(LogClass, Log, TEXT("ERROR: End node not found"));
        return TArray<FVector>();
    }
    
    if(start == end){
        UE_LOG(LogClass, Log, TEXT("ERROR: Start node is end node"));
        TArray<FVector> arr;
        arr.Add(start->Position);
        arr.Add(end->Position);
        return arr;
    }
    
    //Create an Open and Closed List for the A* search
    TArray<PathNode*> OpenList,
        ClosedList;
    
    //Check Path available
    if(AStarAlgorithm(start, end, NodeMap, OpenList, ClosedList)){
        UE_LOG(LogClass, Log, TEXT("PATH IS AVAILABLE!!!"));
        return GeneratePath(start, end, ClosedList);
    }
    else{
        UE_LOG(LogClass, Log, TEXT("ERROR MAKING PATH"));
        return TArray<FVector>();
    }
}


/**
 * Actual A* Algorithm
 */
bool AStar::AStarAlgorithm(PathNode *StartNode, PathNode *FinalNode, TArray<PathNode*> &NodeList, TArray<PathNode*> &OpenList, TArray<PathNode*> &ClosedList){
    //Check there are actually nodes to traverse
    if(NodeList.Num() == 0){
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
        PathNode *Current = GetMinCostNode(OpenList);
        
        //Check node is real
        if(!Current){
            UE_LOG(LogClass, Log, TEXT("NODE NOT FOUND"));
            return false;
        }
        
        //Remove current node from Open List (already explored)
        OpenList.Remove(Current);
        
        //Check if Node has children, if not find them
        if(Current->Connected.empty()){
            bool isConnected = FindNodeConnections(Current, NodeList);
            
            //Check node actually has connections, reject it if it doesn't
            if(!isConnected){
                UE_LOG(LogClass, Log, TEXT("NO CONNECTIONS TO NODE, ID: %s"), *FString::FromInt(Current->ID));
                continue;
            }
        }
        
        //Get G value of Current Node
        const int GCostCurrent = Current->Cost - HeuristicCost(Current->Position, FinalNode->Position);
        
        //Iterate through all of the connected nodes
        for(int i = 0; i < Current->Connected.size(); i++){
            //Get Child node
            PathNode *Successor = GetMatchingNodeByID(Current->Connected[i], NodeList);
            if(!Successor){
                continue;
            }
            
            //If the child node is the goal, we have a winner
            if(Successor->ID == FinalNode->ID){
                UE_LOG(LogClass, Log, TEXT("WE HAVE A PATH"));
                Successor->Parent = Current;
                ClosedList.AddUnique(Current);
                return true;
            }
            
            //Check if node is actually possible to go through
            if(!Successor->bIsPassable){
                //Ignore if it isn't
                continue;
            }
            
            //Recalculate Cost to reach node (Add G cost of current node)
            const int cost = GCostCurrent + CostToMove(Current->Position, Successor->Position) + HeuristicCost(Successor->Position, FinalNode->Position);
            
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
            OpenList.AddUnique(Successor);
            num++;
        }
        
        //Add Current node to closed list
        ClosedList.AddUnique(Current);
        num++;
    }
    
    //If reached here, no path exists to player
    UE_LOG(LogClass, Log, TEXT("NO POSSIBLE PATH, after %s iterations"), *FString::FromInt(num));
    return false;
}


/**
 * Generates the FVector Array of positions to in the path
 */
TArray<FVector> AStar::GeneratePath(PathNode *StartNode, PathNode *FinalNode, const TArray<PathNode*> &ClosedList){
    //Array containing final nodes (vector as can be reversed)
    vector<PathNode*> FinalNodes;
    
    //Final path to Player
    TArray<FVector> Path;
    
    //Start from final node and make your way upwards
    PathNode *R = FinalNode;
    
    //Safety int to prevent infinite loop
    int num = 0;
    
    while(R && R->ID != StartNode->ID && num < 1000){
        FinalNodes.push_back(R);
        R = GetMatchingNodeByID(R->Parent->ID, ClosedList);
        num++;
    }
    
    //Since Nodes have been added in reverse order, need to reverse the array
    reverse(FinalNodes.begin(), FinalNodes.end());
    
    
    //Add the positions the AI has to traverse
    for(auto Node : FinalNodes){
        Path.Add(Node->Position);
    }
    
    
    return Path;
}

/**
 * Determines Heuristic cost of moving from one position to another, using Manhattan Distance
 */
int AStar::HeuristicCost(const FVector &Source, const FVector &Destination){
    FVector dist = Source - Destination;
    return (int)(FMath::Abs(dist.X) + FMath::Abs(dist.Y) + FMath::Abs(dist.Z));
}


/**
 * Determines cost of moving from one position to another, using Manhattan Distance
 */
int AStar::CostToMove(const FVector &Source, const FVector &Destination){
    FVector dist = Source - Destination;
    return (int)(FMath::Abs(dist.X) + FMath::Abs(dist.Y) + FMath::Abs(dist.Z));
}


/**
 * Populates Neighboring nodes list for given Node
 */
bool AStar::FindNodeConnections(PathNode *Node, TArray<PathNode*> &List){
    //Empty list currently there
    Node->Connected.clear();
    
    float NodeX = Node->Position.X,
    NodeY = Node->Position.Y;
    
    
//    for(float x = NodeX - NODE_DISTANCE; x < NodeX + NODE_DISTANCE + 1; x += NODE_DISTANCE){
//        for(float y = NodeY - NODE_DISTANCE; y < NodeY + NODE_DISTANCE + 1; y += NODE_DISTANCE){
//            //Skip if location is current node's
//            if(x == NodeX && y == NodeY){
//                continue;
//            }
//            
//            PathNode* found = GetNodeWithXY(x, y, List);
//            
//            if(found){
//                Node->Connected.push_back(found->ID);
//            }
//        }
//    }
    
    PathNode *P0 = GetNodeWithXY(NodeX, NodeY + NODE_DISTANCE, List);
    if(P0){
        Node->Connected.push_back(P0->ID);
    }
    PathNode *P1 = GetNodeWithXY(NodeX + NODE_DISTANCE, NodeY, List);
    if(P1){
        Node->Connected.push_back(P1->ID);
    }
    PathNode *P2 = GetNodeWithXY(NodeX - NODE_DISTANCE, NodeY, List);
    if(P2){
        Node->Connected.push_back(P2->ID);
    }
    PathNode *P3 = GetNodeWithXY(NodeX, NodeY - NODE_DISTANCE, List);
    if(P3){
        Node->Connected.push_back(P3->ID);
    }
    
    
    //	UE_LOG(LogClass, Log, TEXT("%s CONNECTIONS to ID: %s"), *FString::FromInt(Node->Connected.size()),*FString::FromInt(Node->ID));
    //UE_LOG(LogClass, Log, TEXT("CONNECTIONS to ID: %s %s"), *FString::FromInt(Node->Connected.size()), Node->Connected.empty() ? *FString("DONT EXIST") : *FString("EXIST"));
    
    //Check if any connections made
    return Node->Connected.empty() ? false : true;
}


/*
 *  ----------------------------------------------------------
 *	---------------PathNode Helper Functions------------------
 *	---------------------------------------------------------- 
 */
PathNode* AStar::GetMatchingNodeByID(const int ID, const TArray<PathNode*> &List){
    for(auto Node : List){
        if(Node->ID == ID){
            return Node;
        }
    }
    
    return nullptr;
}


PathNode* AStar::GetMinCostNode(const TArray<PathNode*> &List){
    PathNode *Min = List.Num() > 0 ? List[0] : nullptr;

    for(auto Node : List){
        if(Node->Cost < Min->Cost){
            Min = Node;
        }
    }
    
    return Min;
}


PathNode* AStar::GetNodeWithXY(const float x, const float y, const TArray<PathNode*> &List){
    for(auto Node : List){
        if(Node->Position.X == x && Node->Position.Y == y){
            return Node;
        }
    }
    
    return nullptr;
}


PathNode* AStar::GetClosestNode(const FVector &Pos, const TArray<PathNode*> &List){
    PathNode *Closest = List.Num() > 0 ? List[0] : nullptr;
    
    //Set large value as default smallest distance
    float dist = 999999.0f;
    
    for(auto Node : List){
        if(!Node->bIsPassable){
            continue;
        }
        
        float DistToNode = FMath::Abs(FVector::Dist(Pos, Node->Position));
        
        if(DistToNode < dist){
            Closest = Node;
            dist = DistToNode;
        }

    }
    
    return Closest;
}

