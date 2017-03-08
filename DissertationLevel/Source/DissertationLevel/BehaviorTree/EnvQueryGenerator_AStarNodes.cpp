// Fill out your copyright notice in the Description page of Project Settings.

#include "../DissertationLevel.h"
#include "EnvQueryGenerator_AStarNodes.h"
#include "../AI/EnemyAIController.h"


UEnvQueryGenerator_AStarNodes::UEnvQueryGenerator_AStarNodes(){

}


void UEnvQueryGenerator_AStarNodes::GenerateItems(FEnvQueryInstance &QueryInstance) const{
	//This array will hold a reference to all the generated items, meaning, the cone items
	TArray<FNavLocation> ItemCandidates;

	//Get a reference for our AI Pawn
	//AActor* AIPawn = Cast<AActor>((QueryInstance.Owner).Get());

	AEnemyAIController *AICon = Cast<AEnemyAIController>(
		(Cast<AActor>(
			(QueryInstance.Owner).Get())->GetInstigatorController())
		);

	//Store its location and its forward vector
	FVector PawnLocation = AICon->GetPawn()->GetActorLocation();
	FVector PawnForwardVector = AICon->GetPawn()->GetActorForwardVector();

	for(float point = -Bounds; point < Bounds; point += PointsSeperation){
		FVector Pos = PawnLocation + FVector(point, 0.0f, 0.0f);
		

		for(int32 p = 0; p < HalfLen; p++){
			//Generate a point for this particular angle and distance
			FNavLocation NavLoc = FNavLocation(PawnLocation + Pos * p);
			UE_LOG(LogTemp, Warning, TEXT("Pos: %s"), *(NavLoc.Location.ToString()));
			//Add the new point into our array
			ItemCandidates.Add(NavLoc);

			AICon->AddToLocations(NavLoc.Location);
		}
	}

	//Projects all the nav points into our Viewport and removes those outside of our navmesh
	ProjectAndFilterNavPoints(ItemCandidates, QueryInstance);

	//Store the generated points as the result of our Query
	StoreNavPoints(ItemCandidates, QueryInstance);
		
	

	////If the angle step is zero we're going into an infinite loop. 
	////Since we don't want that, don't execute the following logic
	//if(AngleStep == 0){
	//	return;
	//}

	//for(float Angle = -ConeDegrees; Angle < ConeDegrees; Angle += AngleStep){
	//	//Start from the left side of the pawn and rotate its forward vector by Angle + 1
	//	FVector LeftVector = PawnForwardVector.RotateAngleAxis(Angle + 1, FVector(0, 0, 1));
	//	//The Left Vector is showing a straight line for that angle. The only thing we need
	//	//is to generate items in that line

	//	TArray<AActor*> Actors;
	//	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), Actors);
	//	bool bIsOccupied = false;

	//	/*for(auto Actor : Actors){
	//		if(Actor->GetActorLocation().X >= (LeftVector.X - 0.5f) 
	//			&& Actor->GetActorLocation().X <= (LeftVector.X + 0.5f)
	//			&& Actor->GetActorLocation().Y >= (LeftVector.Y - 0.5f)
	//			&& Actor->GetActorLocation().Y <= (LeftVector.Y + 0.5f)){
	//			bIsOccupied = true;
	//			break;
	//		}
	//	}*/

	//	if(bIsOccupied){
	//		continue;
	//	}

	//	//Generates all the points for the current line (LeftVector)
	//	for(int32 Point = 0; Point < ConeRadius; Point++){
	//		//Generate a point for this particular angle and distance
	//		FNavLocation NavLoc = FNavLocation(PawnLocation + LeftVector * Point * PointsDistance);

	//		//Add the new point into our array
	//		ItemCandidates.Add(NavLoc);

	//		AICon->AddToLocations(NavLoc.Location);
	//	}
	//}

	////Projects all the nav points into our Viewport and removes those outside of our navmesh
	//ProjectAndFilterNavPoints(ItemCandidates, QueryInstance);

	////Store the generated points as the result of our Query
	//StoreNavPoints(ItemCandidates, QueryInstance);
}
