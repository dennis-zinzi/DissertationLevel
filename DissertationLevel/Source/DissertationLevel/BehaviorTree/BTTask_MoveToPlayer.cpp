// Fill out your copyright notice in the Description page of Project Settings.

#include "../DissertationLevel.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "../AI/EnemyCharacter.h"
#include "../AI/EnemyAIController.h"
#include "../DissertationLevelCharacter.h"
#include "../DissertationLevelGameMode.h"

#include "BTTask_MoveToPlayer.h"

#define IN_RANGE_DISTANCE 1000.0f
#define CAUGHT_DISTANCE 80.0f


EBTNodeResult::Type UBTTask_MoveToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory){
	//Get AI controller
	AEnemyAIController *EnemyPC = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());

	//Get Player Character
	ADissertationLevelCharacter *Player = Cast<ADissertationLevelCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());

	//Blackboard key name representing player being chased
	const FName key = "Target";

	if(Player){
		FVector PlayerPos = Player->GetActorLocation(),
			EnemyPos = EnemyPC->GetPawn()->GetActorLocation();

		//Get distance between AI and player
		const float dist = FVector::Dist(EnemyPos, PlayerPos);

		//Check if AI caught the player
		if(dist <= CAUGHT_DISTANCE){
			EnemyPC->StopBehavior();
            EnemyPC->StopMovement();
			//Set Game to Lost
			//((ADissertationLevelGameMode *)GetWorld()->GetAuthGameMode())->SetCurrentState(EPlayState::EGameOver);
			
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "YOU LOSE X_X");
			return EBTNodeResult::Succeeded;
		}
		//Check if AI within "viewing" distance
		else if(dist < IN_RANGE_DISTANCE){
            //Get the Node Map
            TArray<PathNode*> MapNodes = ((ADissertationLevelGameMode *)GetWorld()->GetAuthGameMode())->GetMapNodes();
            
			//Make A* path
			EnemyPC->ChasePlayer(Player, MapNodes);

			return EBTNodeResult::Succeeded;
		}
		//Player survived the chase
		else{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Emerald, "Player got away safely");
			//Stop chasing
			EnemyPC->StopMovement();
		}
	}

	//Clear target value
	OwnerComp.GetBlackboardComponent()->ClearValue(key);
	return EBTNodeResult::Failed;
}


