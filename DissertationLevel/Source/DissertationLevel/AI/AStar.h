// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "PathNode.h"

/**
 * 
 */
class DISSERTATIONLEVEL_API AStar
{
    public:
    
        AStar();
        ~AStar();
    
        //Generates Level GridMap
        static TArray<PathNode*> CreateGridMap();
    
        //Determines unpassable nodes and updates the given list
        static void CheckOverlappingNodes(const FVector &Start, const FVector &End, const TArray<PathNode*> &List);
    
        //Get A* Vector list of locations to move through
        static TArray<FVector> GetAStarPath(const FVector &StartPos, const FVector &EndPos, TArray<PathNode*> &NodeMap);
    
        //Gets closest node to position
        static PathNode* GetClosestNode(const FVector &Pos, const TArray<PathNode*> &List);
    
        //Gets closest neighboring node to position
        static PathNode* GetClosestNeighborNode(const FVector &CurPos, const FVector &GoPos, TArray<PathNode*> &List);
    
    private:
    
        static bool AStarAlgorithm(PathNode *StartNode, PathNode *FinalNode, TArray<PathNode*> &NodeList, TArray<PathNode*> &OpenList, TArray<PathNode*> &ClosedList);
        static int HeuristicCost(const FVector &Source, const FVector &Destination);
        static int CostToMove(const FVector &Source, const FVector &Destination);
        
        static TArray<FVector> GeneratePath(PathNode *StartNode, PathNode *FinalNode, const TArray<PathNode*> &ClosedList);
        
        static PathNode* GetMinCostNode(const TArray<PathNode*> &List);
        static PathNode* GetMatchingNodeByID(const int ID, const TArray<PathNode*> &List);
        static PathNode* GetNodeWithXY(const float x, const float y, const TArray<PathNode*> &List);
    
        static bool FindNodeConnections(PathNode *Node, TArray<PathNode*> &List);
};
