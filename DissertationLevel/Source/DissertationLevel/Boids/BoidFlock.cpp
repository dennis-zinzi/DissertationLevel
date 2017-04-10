// Fill out your copyright notice in the Description page of Project Settings.

#include "../DissertationLevel.h"

#include "../AI/EnemyAIController.h"
#include "../AI/AStar.h"

#include "BoidFlock.h"

//Percentage
#define COHESION_FACTOR 0.1f
#define OVERLAP_DISTANCE 100.0f
#define ALIGNMENT_FACTOR 0.05f

#define VELOCITY_LIMIT 350.0f
#define LOCATION_FACTOR 0.01f

BoidFlock::BoidFlock(TArray<AEnemyCharacter*> AIs, AWinningLocation *WinLoc, TArray<PathNode*> MapNodes){
	//Get every AI character
	this->AIs = AIs;
    this->WinLoc = WinLoc;
    this->MapNodes = MapNodes;
    
    for(auto Boid : AIs){
        PerceivedCenter += Boid->GetActorLocation();
    }
    
    PerceivedCenter /= AIs.Num();
}

BoidFlock::~BoidFlock(){
}

void BoidFlock::UpdateAIPositions(){
	FVector Cohesion,
		Allignment,
		Separation,
        GoalTendency;
    
    //Calculate A* path from Perceived Center to Goal location
    
    
    //for(auto locToGo : Locations){
	for(auto AI : AIs){
		Cohesion = CalculateBoidCohesion(AI);
		Allignment = CalculateBoidAlignment(AI);
		Separation = CalculateBoidSeparation(AI);
        GoalTendency = CalculateGoalTendency(AI, WinLoc->GetActorLocation());

        AI->GetCharacterMovement()->Velocity += Cohesion + Allignment + Separation + GoalTendency;
        LimitVelocity(AI);
        
        AEnemyAIController *Cont = Cast<AEnemyAIController>(AI->GetController());
        
        if(Cont){
//            Cont->MoveToLocation(AI->GetCharacterMovement()->Velocity + AI->GetActorLocation());
            Cont->MoveToLocation(AStar::GetClosestNode(AI->GetCharacterMovement()->Velocity + AI->GetActorLocation(), MapNodes)->Position);
        }
	}
    //}
}

FVector BoidFlock::CalculateBoidCohesion(AEnemyCharacter *AI){
//	FVector PerceivedCenter;
    PerceivedCenter = FVector::ZeroVector;

	for(auto Boid : AIs){
		if(Boid == AI){
			continue;
		}

		PerceivedCenter += Boid->GetActorLocation();
	}

	PerceivedCenter /= AIs.Num() - 1;
    
    return (PerceivedCenter - AI->GetActorLocation()) * COHESION_FACTOR;
}


FVector BoidFlock::CalculateBoidAlignment(AEnemyCharacter *AI){
	FVector PereceivedVel;

	for(auto Boid : AIs){
		if(Boid == AI){
			continue;
		}

		PereceivedVel += Boid->GetCharacterMovement()->Velocity;
	}

	PereceivedVel /= AIs.Num() - 1;

	return (PereceivedVel - AI->GetCharacterMovement()->Velocity) * ALIGNMENT_FACTOR;
}


FVector BoidFlock::CalculateBoidSeparation(AEnemyCharacter *AI){
	FVector Separation(0.0f, 0.0f, 0.0f);

	for(auto Boid : AIs){
		if(Boid == AI){
			continue;
		}

		if(FMath::Abs(FVector::Dist(Boid->GetActorLocation(), AI->GetActorLocation())) < OVERLAP_DISTANCE){
			Separation -= (Boid->GetActorLocation() - AI->GetActorLocation());
		}
	}

	return Separation;
}


FVector BoidFlock::CalculateGoalTendency(AEnemyCharacter *AI, const FVector &PosToGo){
    return (PosToGo - AI->GetActorLocation()) * LOCATION_FACTOR;
}


void BoidFlock::LimitVelocity(AEnemyCharacter *AI){
	FVector Vel = AI->GetCharacterMovement()->Velocity;
    UE_LOG(LogClass, Log, TEXT("VELOCITY: %s"), *FString::SanitizeFloat(Vel.Size()));
	if(Vel.Size() > VELOCITY_LIMIT){
		Vel = (Vel / Vel.Size()) * VELOCITY_LIMIT;
	}
	
    AI->GetCharacterMovement()->Velocity = Vel;
}





