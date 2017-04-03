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
