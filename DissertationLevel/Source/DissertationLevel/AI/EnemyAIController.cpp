// Fill out your copyright notice in the Description page of Project Settings.

#include "../DissertationLevel.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "EnemyCharacter.h"
#include "EnemyPatrolPoint.h"

#include "EnemyAIController.h"


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


void AEnemyAIController::SetPlayerCaught(APawn * Pawn){
	if(BlackboardComp){
		BlackboardComp->SetValue<UBlackboardKeyType_Object>(EnemyKeyID, Pawn);
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, "YOU'VE BEEN SPOTTED!");
		BehaviorComp->RestartTree();
	}
}


void AEnemyAIController::StopBehavior(){
	BehaviorComp->StopTree();
}


void AEnemyAIController::PopulateOpenList(TArray<FVector> &Locations){
	float PointsDistance = 50.0f;

	for(auto pos : Locations){
		FNodeCost nc = {pos};
		OpenList.Add(nc);
	}
}


bool AEnemyAIController::AStarAlgorithm(TArray<FVector> &Locations, TArray<FNodeCost*> &OpenList, TArray<FNodeCost*> &ClosedList, const FVector &CurrentLoc, const FVector &Destination){
	Initialization(Locations, OpenList, ClosedList, CurrentLoc, Destination);

	if(OpenList.Num() == 0){
		return false;
	}
	
	FNodeCost P = *OpenList[0];

	while(OpenList.Num() > 0){
		if(P.position == Destination){
			GeneratePath(ClosedList, CurrentLoc, Destination);
			return true;
		}

		int connections = P.connected.Num();
		int BestCost = 0;
		for(int i = 0; i < connections; i++){
			FNodeCost Q = P.connected[i];
			Q.cost = HeuristicCost(Q.position, Destination) + CostToMove(P.position, Q.position);

			if(OpenList.Contains(&Q) && Q.cost >= BestCost){
				continue;
			}
			else if(OpenList.Contains(&Q) && Q.cost < BestCost){
				BestCost = Q.cost;
				Q.parent = &P;
			}
			else{
				Q.parent = &P;
				OpenList.Add(&Q);
			}
		}

		OpenList.Remove(&P);
		ClosedList.Add(&P);

		P = GetMinCostNode(OpenList);
	}

	//If reached here, no Path exists
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


void AEnemyAIController::Initialization(TArray<FVector> &Locations, TArray<FNodeCost*> &OpenList, TArray<FNodeCost*> &ClosedList, const FVector &CurrentLoc, const FVector &Destination) {
	float G = CostToMove(CurrentLoc, CurrentLoc),
		H = HeuristicCost(CurrentLoc, Destination);

	FNodeCost start = {CurrentLoc, G + H};

	OpenList.Add(&start);
}

TArray<FVector> AEnemyAIController::GeneratePath(TArray<FNodeCost*> &ClosedList, const FVector &CurrentLoc, const FVector &Destination){
	//Final path to Player
	TArray<FVector> Path;

	FNodeCost R = {Destination};

	while(R.position != CurrentLoc){
		Path.Add(R.position);
		R = *R.parent;
	}

	//Final position to add is players
	Path.Add(CurrentLoc);

	return Path;
}


FNodeCost AEnemyAIController::GetMinCostNode(const TArray<FNodeCost*> &List){
	FNodeCost Min = *List[0];

	for(int i = 0; i < List.Num(); i++){
		if((*List[i]).cost < Min.cost){
			Min = *List[i];
		}
	}

	return Min;
}