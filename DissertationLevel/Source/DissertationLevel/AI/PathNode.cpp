// Fill out your copyright notice in the Description page of Project Settings.

#include "../DissertationLevel.h"
#include "PathNode.h"


PathNode::PathNode(int ID, const FVector &Position, int Cost, TArray<int> Connected, PathNode *Parent){
	this->ID = ID;
	this->Position = Position;
	this->Cost = Cost;
	this->Connected = Connected;
	this->Parent = Parent;
}

PathNode::PathNode(const FVector &v){
	ID = GetVectorID(v);
	Position = v;
	Cost = 0;
	Connected = TArray<int>();
	Parent = nullptr;
}

PathNode::~PathNode(){
}

int PathNode::GetVectorID(const FVector &v) const{
	return (int)(FMath::Abs(v.X) + FMath::Abs(v.Y) + FMath::Abs(v.Z));
}
