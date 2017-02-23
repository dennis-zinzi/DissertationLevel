// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "EnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class DISSERTATIONLEVEL_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
	UPROPERTY(Transient)
	class UBlackboardComponent *BlackboardComp;

	UPROPERTY(Transient)
	class UBehaviorTreeComponent *BehaviorComp;

	public:
		AEnemyAIController();
		virtual void Possess(APawn *InPawn) override;

		inline uint8 GetEnemyKeyID() const{
			return EnemyKeyID;
		}

	private:
		uint8 EnemyKeyID;
	
};
