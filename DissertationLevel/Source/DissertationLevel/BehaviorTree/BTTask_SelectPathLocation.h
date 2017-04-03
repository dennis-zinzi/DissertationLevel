// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SelectPathLocation.generated.h"

/**
 * 
 */
UCLASS()
class DISSERTATIONLEVEL_API UBTTask_SelectPathLocation : public UBTTaskNode
{
	GENERATED_BODY()
	
	
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;	
};
