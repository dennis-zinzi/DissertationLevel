// Fill out your copyright notice in the Description page of Project Settings.

#include "DissertationLevel.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "EnemyCharacter.h"

#include "EnemyAIController.h"


AEnemyAIController::AEnemyAIController(){
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));
	BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));
}

void AEnemyAIController::Possess(APawn *InPawn){
	Super::Possess(InPawn);

	AEnemyCharacter *Char = Cast<AEnemyCharacter>(InPawn);

	if(Char && Char->AIBehavior){
		BlackboardComp->InitializeBlackboard(*Char->AIBehavior->BlackboardAsset);

		//Get KEyID from Blackboard
		EnemyKeyID = BlackboardComp->GetKeyID("Target");

		BehaviorComp->StartTree(*Char->AIBehavior);
	}
}



