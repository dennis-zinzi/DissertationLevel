// Fill out your copyright notice in the Description page of Project Settings.

#include "DissertationLevel.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "EnemyCharacter.h"
#include "EnemyAIController.h"
#include "DissertationLevelCharacter.h"

#include "BTTask_MoveToPlayer.h"

#define IN_RANGE_DISTANCE 800.0f


EBTNodeResult::Type UBTTask_MoveToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory){
	AEnemyAIController *EnemyPC = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());

	ADissertationLevelCharacter *Player = 
		Cast<ADissertationLevelCharacter>(
			OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Object>(EnemyPC->GetEnemyKeyID())
		);

	if(Player){
		FVector PlayerPos = Player->GetActorLocation(),
			EnemyPos = EnemyPC->GetPawn()->GetActorLocation();

		const float dist = FVector::Dist(EnemyPos, PlayerPos);
		

		//Check if AI within "viewing" distance
		if(FVector::Dist(EnemyPos, PlayerPos) < IN_RANGE_DISTANCE){
			OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Object>(EnemyPC->GetEnemyKeyID(), Player);
			EnemyPC->MoveToActor(Player, 20.0f, true, true, true, 0, true);
		}
		else{
			EnemyPC->StopMovement();
			FName key = "Target";
			OwnerComp.GetBlackboardComponent()->ClearValue(key);
		}

		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, "Player exists");
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}


