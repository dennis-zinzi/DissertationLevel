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


void AEnemyAIController::SetPlayerCaught(APawn * Pawn){
	if(BlackboardComp){
		BlackboardComp->SetValue<UBlackboardKeyType_Object>(EnemyKeyID, Pawn);
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, "YOU'VE BEEN SPOTTED!");
		BehaviorComp->RestartTree();

		ADissertationLevelCharacter *Player = Cast<ADissertationLevelCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());

		if(Player){
			FVector PlayerPos = Player->GetActorLocation();

			//Get distance between AI and player
			const float dist = FVector::Dist(GetPawn()->GetActorLocation(), PlayerPos);

			//Check if AI caught the player
			if(dist <= CAUGHT_DISTANCE){
				//Set Game to Lost
				((ADissertationLevelGameMode *)GetWorld()->GetAuthGameMode())->SetCurrentState(EPlayState::EGameOver);

				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "YOU LOSE X_X");
			}
			//Check if AI within "viewing" distance
			else if(dist < IN_RANGE_DISTANCE){
				TArray<FVector> Locations = GetAStarPath();
				if(Locations.Num() > 0){
					for(int i = 0; i < Locations.Num(); i++){
						MoveToLocation(Locations[i], 25.0f, true, false, true, true, 0, true);
					}
					UE_LOG(LogClass, Log, TEXT("PATHING!"));
				}

				UE_LOG(LogClass, Log, TEXT("PATHING?"));
				//EnemyPC->MoveToActor(Player, 20.0f, true, true, true, 0, true);
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, "Pursuiting Player!!!");
			}
			//Player survived the chase
			else{
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Emerald, "Player got away safely");
				//Stop chasing
				StopMovement();
			}
		}
	}
}


void AEnemyAIController::StopBehavior(){
	BehaviorComp->StopTree();
}


void AEnemyAIController::PopulateOpenList(){
	float PointsDistance = 50.0f;

	for(int i = 0; i < Nodes.Num(); i++){
		PathNode *nc = Nodes[i];

		for(int j = 0; j < Nodes.Num(); j++){
			if(j == i){ 
				continue;
			}
			
			if(FVector::Dist(nc->Position, Nodes[j]->Position) <= PointsDistance){
				nc->Connected.Add(Nodes[j]->ID);
			}
		}

		OpenList.Add(nc);
	}
}


bool AEnemyAIController::AStarAlgorithm(const PathNode &CurrentNode, const PathNode &FinalNode){
	Initialization(CurrentNode, FinalNode);

	if(OpenList.Num() == 0){
		return false;
	}
	
	PathNode *P = GetMinCostNode(OpenList);

	if(!P || P == nullptr){
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Purple, "NO NODES ON OPEN LIST");
		return false;
	}

	while(OpenList.Num() > 0){
		if(P->ID == FinalNode.ID){
			LocationsToGo = GeneratePath(CurrentNode, FinalNode);
			return true;
		}

		if(P->Connected.Num() > 0){
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Purple, "NO CONNECTION TO NODE");
			return false;
		}
		
		int BestCost = 0;

		for(int i = 0; i < P->Connected.Num(); i++){
			PathNode *Q = GetMatchingNode(P->Connected[i], OpenList);//*OpenList.FindByPredicate([P,i](PathNode *nc){ return nc->ID == P->connected[i]; }); //Nodes
			Q->Cost = HeuristicCost(Q->Position, FinalNode.Position) + CostToMove(P->Position, Q->Position);

			if(OpenList.Contains(Q) && Q->Cost >= BestCost){
				continue;
			}
			else if(OpenList.Contains(Q) && Q->Cost < BestCost){
				BestCost = Q->Cost;
				Q->Parent = P;
			}
			else{
				Q->Parent = P;
				OpenList.Add(Q);
			}
		}

		OpenList.Remove(P);
		ClosedList.Add(P);
		
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


void AEnemyAIController::Initialization(const PathNode &CurrentNode, const PathNode &FinalNode) {
	float G = CostToMove(CurrentNode.Position, CurrentNode.Position),
		H = HeuristicCost(CurrentNode.Position, FinalNode.Position);

	PathNode start(CurrentNode.ID, CurrentNode.Position, G + H);
	OpenList.Add(&start);
}

TArray<FVector> AEnemyAIController::GeneratePath(const PathNode &CurrentNode, const PathNode &FinalNode){
	//Final path to Player
	TArray<FVector> Path;

	PathNode R = FinalNode;

	while(R.ID != FinalNode.ID){
		Path.Add(R.Position);
		R = *R.Parent;
	}

	//Final position to add is players
	Path.Add(CurrentNode.Position);

	return Path;
}


PathNode* AEnemyAIController::GetMinCostNode(const TArray<PathNode*> &List){
	PathNode *Min = List.Num() ? List[0] : nullptr;

	for(int i = 0; i < List.Num(); i++){
		if(List[i]->Cost < Min->Cost){
			Min = List[i];
		}
	}

	return Min;
}


int AEnemyAIController::GetVectorID(const FVector &pos) const{
	return (int)(FMath::Abs(pos.X) + FMath::Abs(pos.Y) + FMath::Abs(pos.Z));
}


TArray<FVector> AEnemyAIController::GetAStarPath(){
	ADissertationLevelCharacter *Player = Cast<ADissertationLevelCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());

	PopulateOpenList();
	if(AStarAlgorithm(PathNode(GetPawn()->GetActorLocation()), PathNode(Player->GetActorLocation()))){
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, "SUCCESS!!!");
		UE_LOG(LogClass, Log, TEXT("WE MADE IT?"));
		return LocationsToGo;
	}
	else{
		UE_LOG(LogClass, Log, TEXT("DNOOOOO!!!!!"));
		return TArray<FVector>();
	}
}


PathNode* AEnemyAIController::GetMatchingNode(const int ID, const TArray<PathNode*> &List){
	UE_LOG(LogClass, Log, TEXT("ID to Find: %s"), *FString::FromInt(ID));
	UE_LOG(LogClass, Log, TEXT("Nodes in List: %s"), *FString::FromInt(List.Num()));
	for(int i = 0; i < List.Num(); i++){
		UE_LOG(LogClass, Log, TEXT("Current ID: %s"), *FString::FromInt(List[i]->ID));
		if(List[i]->ID == ID){
			return List[i];
		}
	}

	return nullptr;
}