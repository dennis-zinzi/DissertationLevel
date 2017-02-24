// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SelectPatrolPoint.generated.h"

/**
 * 
 */
UCLASS()
class DISSERTATIONLEVEL_API UBTTask_SelectPatrolPoint : public UBTTaskNode
{
	GENERATED_BODY()
	
	public:
		virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	
};
