// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/BTService.h"
#include "BTService_CheckForPlayer.generated.h"

/**
 * 
 */
UCLASS()
class DISSERTATIONLEVEL_API UBTService_CheckForPlayer : public UBTService
{
	GENERATED_BODY()
	
	public:
		UBTService_CheckForPlayer();

	//protected:
		virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
	
};
