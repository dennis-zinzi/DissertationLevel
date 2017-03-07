// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EnvironmentQuery/EnvQueryTest.h"
#include "AStarQueryTest.generated.h"

/**
 * 
 */
UCLASS()
class DISSERTATIONLEVEL_API UAStarQueryTest : public UEnvQueryTest
{
	GENERATED_BODY()
	
	//public:
		virtual void RunTest(FEnvQueryInstance &QueryInstance) const override;
	
	private:
		float GetHeuristicCost(AActor *AI, AActor *Player) const;
		float GetCostToMove(AActor *AI, FVector MapPos) const;

		UPROPERTY(EditDefaultsOnly, Category = "Behavior")
		TSubclassOf<UEnvQueryContext> Context;

		UPROPERTY()
		/** Validation: item type that can be used with this test */
		TSubclassOf<UEnvQueryItemType> ValidItemType;
};
