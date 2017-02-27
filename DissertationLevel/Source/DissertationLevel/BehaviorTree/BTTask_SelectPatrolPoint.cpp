// Fill out your copyright notice in the Description page of Project Settings.

#include "DissertationLevel.h"

#include "EnemyPatrolPoint.h"
#include "EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"

#include "BTTask_SelectPatrolPoint.h"


EBTNodeResult::Type UBTTask_SelectPatrolPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory){
	AEnemyAIController *AIController = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());

	if(AIController){
		//Get Blackboard Component
		UBlackboardComponent *BlackboardComp = OwnerComp.GetBlackboardComponent();//->GetValue<UBlackboardKeyType_Object>(EnemyPC->GetEnemyKeyID());

		AEnemyPatrolPoint *CurrentPoint = Cast<AEnemyPatrolPoint>(BlackboardComp->GetValue<UBlackboardKeyType_Object>(AIController->GetPatrolPointKeyID()));
		
		TArray<AActor*> AvailablePatrolPoints = AIController->GetPatrolPoints();

		AEnemyPatrolPoint *NextPatrolPoint = nullptr;

		if(AIController->GetCurrentPatrolPoint() != AvailablePatrolPoints.Num() - 1){
			//Increment current patrol point 
			AIController->SetCurrentPatrolPoint(AIController->GetCurrentPatrolPoint() + 1);
			NextPatrolPoint = Cast<AEnemyPatrolPoint>(AvailablePatrolPoints[AIController->GetCurrentPatrolPoint()]);
		}
		//If no more patrol points to go to
		else{
			NextPatrolPoint = Cast<AEnemyPatrolPoint>(AvailablePatrolPoints[0]);
			AIController->SetCurrentPatrolPoint(0);
		}

		BlackboardComp->SetValueAsObject("LocationToGo", NextPatrolPoint);

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}

