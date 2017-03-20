// Fill out your copyright notice in the Description page of Project Settings.

#include "../DissertationLevel.h"
#include "PathNode.h"


PathNode::PathNode(int ID, const FVector &Position, int Cost, vector<int> Connected, PathNode *Parent, bool bIsPassable){
	this->ID = ID;
	this->Position = Position;
	this->Cost = Cost;
	this->Connected = Connected;
	this->Parent = Parent;
	this->bIsPassable = bIsPassable;
}


PathNode::~PathNode(){
}


void PathNode::CheckOverlappingNodes(const FVector &Start, const FVector &End, const TArray<PathNode*> &List){
	for(auto Node : List){
		if(Start.X <= Node->Position.X && Node->Position.X <= End.X
			&& Start.Y <= Node->Position.Y && Node->Position.Y <= End.Y){
			//If checks above true, must be overlapping, so set isPassable flag to false
			Node->bIsPassable = false;
		}
	}
}