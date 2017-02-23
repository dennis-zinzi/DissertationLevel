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


EBTNodeResult::Type UBTTask_MoveToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory){
	AEnemyAIController *EnemyPC = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());

	ADissertationLevelCharacter *Player = 
		Cast<ADissertationLevelCharacter>(
			OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Object>(EnemyPC->GetEnemyKeyID())
		);

	if(Player){
		EnemyPC->MoveToActor(Player, 20.0f, true, true, true, 0, true);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}


