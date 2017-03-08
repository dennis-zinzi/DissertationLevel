// Fill out your copyright notice in the Description page of Project Settings.

#include "../DissertationLevel.h"

#include "../DissertationLevelCharacter.h"
#include "../AI/EnemyAIController.h"
#include "../Game/WorldObject.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_VectorBase.h"

#include "AStarQueryTest.h"




void UAStarQueryTest::RunTest(FEnvQueryInstance &QueryInstance) const{
	TArray<AActor*> Actors;
	QueryInstance.GetAllAsActors(Actors);

	TArray<FVector> Locations;
	if(!QueryInstance.PrepareContext(Context, Locations)){
		return;
	}

	for(FEnvQueryInstance::ItemIterator it(this, QueryInstance); it; ++it){
		AActor *ActorInPos = GetItemActor(QueryInstance, it.GetIndex());

		if(Cast<ADissertationLevelCharacter>(ActorInPos)){
			it.SetScore(EEnvTestPurpose::Score, EEnvTestFilterType::Range, 1.0f, 0.0f, 1.0f);
		}
		else if(Cast<AWorldObject>(ActorInPos)){
			it.SetScore(EEnvTestPurpose::Score, EEnvTestFilterType::Range, 0.0f, 0.0f, 1.0f);
		}
		else{
			AEnemyAIController *AICon = Cast<AEnemyAIController>(
				(Cast<AActor>(
					(QueryInstance.Owner).Get())->GetInstigatorController())
				);

			ADissertationLevelCharacter *player = Cast<ADissertationLevelCharacter>(AICon->GetBlackboardComp()->GetValueAsObject(FName("Target")));

			if(player){
				float dist = FMath::Abs(FVector::Dist(AICon->GetPawn()->GetActorLocation(), player->GetActorLocation()));
				float distToItem = FVector::Dist(AICon->GetPawn()->GetActorLocation(), GetItemLocation(QueryInstance, it.GetIndex()));

				float score = distToItem / dist;

				it.SetScore(EEnvTestPurpose::Score, EEnvTestFilterType::Range, score, 0.0f, 1.0f);
			}
		}
	}
}


float UAStarQueryTest::GetHeuristicCost(AActor *AI, AActor *Player) const{
	FVector dist = AI->GetActorLocation() - Player->GetActorLocation();
	return FMath::Abs(dist.X) + FMath::Abs(dist.Y) + FMath::Abs(dist.Z);
}


float UAStarQueryTest::GetCostToMove(AActor *AI, FVector MapPos) const{
	FVector dist = AI->GetActorLocation() - MapPos;
	return FMath::Abs(dist.X) + FMath::Abs(dist.Y) + FMath::Abs(dist.Z);
}