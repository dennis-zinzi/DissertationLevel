// Fill out your copyright notice in the Description page of Project Settings.

#include "../DissertationLevel.h"
#include "EnvQueryGenerator_AStarNodes.h"
#include "../AI/EnemyAIController.h"
#include "../AI/PathNode.h"
#include "../DissertationLevelCharacter.h"


UEnvQueryGenerator_AStarNodes::UEnvQueryGenerator_AStarNodes(){

}


void UEnvQueryGenerator_AStarNodes::GenerateItems(FEnvQueryInstance &QueryInstance) const{
	//This array will hold a reference to all the generated items, meaning, the grid items
	TArray<FNavLocation> ItemCandidates;

	AEnemyAIController *AICon = Cast<AEnemyAIController>(
		(Cast<AActor>(
			(QueryInstance.Owner).Get())->GetInstigatorController())
		);

	//Clear current nodes
	AICon->ClearLists();

	//Store its location and its forward vector
	FVector PawnLocation = AICon->GetPawn()->GetActorLocation();

	//Rotate Vector by 90 degrees
	FVector LineVector = AICon->GetPawn()->GetActorForwardVector().RotateAngleAxis(-90.0f, FVector(0.0f, 0.0f, 1.0f));

	//PathNode ID
	int ID = 0;

	//X Pos loop
	for(float x = PawnLocation.X - RangeDistance; x < PawnLocation.X + RangeDistance + 1; x += PointsDistance){
		//Y Pos loop
		for(float y = PawnLocation.Y; y < PawnLocation.Y + RangeDistance + 1; y += PointsDistance){
			//Generate Point at these coordinates
			FNavLocation NavLoc = FNavLocation(FVector(x, y, PawnLocation.Z));

			//Add the new point into our array
			ItemCandidates.Add(NavLoc);

			//AICon->AddToLocations(NavLoc.Location);
			PathNode pn(ID, NavLoc.Location);
			AICon->AddToNodesList(pn);
			
			UE_LOG(LogClass, Log, TEXT("Node ID: %s"), *FString::FromInt(pn.ID));

			ID++;
		}
	}

	//Start Node
	FNavLocation StartLoc = FNavLocation(PawnLocation);
	////Add the start and end positions into our array
	ItemCandidates.Add(StartLoc);
	////Add start and end nodes
	PathNode pn(ID + 1, StartLoc.Location);
	AICon->AddToNodesList(pn);

	////End Node (Player Pos)
	if(GetWorld() && GetWorld() != NULL){
		APlayerController *Player = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		FNavLocation EndLoc = FNavLocation(Player->GetPawn()->GetActorLocation());
		ItemCandidates.Add(EndLoc);
		PathNode pn1(ID + 2, EndLoc.Location);
		AICon->AddToNodesList(pn1);
	}

	//Projects all the nav points into our Viewport and removes those outside of our navmesh
	ProjectAndFilterNavPoints(ItemCandidates, QueryInstance);

	//Store the generated points as the result of our Query
	StoreNavPoints(ItemCandidates, QueryInstance);
}
