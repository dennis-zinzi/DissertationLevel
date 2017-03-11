// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <vector>

using std::vector;

/**
 * 
 */
class DISSERTATIONLEVEL_API PathNode
{
	public:
		PathNode(int ID, const FVector &Position, int Cost = 0, vector<int> Connected = vector<int>(), PathNode *Parent = nullptr);
		PathNode(const FVector &v);

		~PathNode();

		int ID;
		FVector Position;
		int Cost;
		//TArray<int> Connected;
		vector<int> Connected;
		PathNode *Parent;

		FORCEINLINE bool operator==(const PathNode &rhs) const{
			if(this == &rhs){
				return true;
			}

			return ID == rhs.ID;
		}

		int GetVectorID(const FVector &v) const;
};
