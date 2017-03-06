// Fill out your copyright notice in the Description page of Project Settings.

#include "../DissertationLevel.h"

#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "../DissertationLevelCharacter.h"
#include "../AI/EnemyAIController.h"

#include "FindPlayerQueryContext.h"



void UFindPlayerQueryContext::ProvideContext(FEnvQueryInstance &QueryInstance, FEnvQueryContextData &ContextData) const{
	Super::ProvideContext(QueryInstance, ContextData);

	AEnemyAIController *AICon = Cast<AEnemyAIController>(
		(Cast<AActor>(
			(QueryInstance.Owner).Get())->GetInstigatorController())
		);

	if(AICon){
		ADissertationLevelCharacter *player = Cast<ADissertationLevelCharacter>(AICon->GetBlackboardComp()->GetValueAsObject(FName("Target")));
		
		if(player){
			UEnvQueryItemType_Actor::SetContextHelper(ContextData, player);
		}
	}
}
