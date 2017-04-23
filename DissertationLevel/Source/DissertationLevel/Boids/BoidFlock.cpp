// Fill out your copyright notice in the Description page of Project Settings.

#include "../DissertationLevel.h"

#include "../AI/EnemyAIController.h"
#include "../AI/AStar.h"

#include "BoidFlock.h"

//Percentage
#define COHESION_FACTOR 0.01f
#define OVERLAP_DISTANCE 200.0f
#define ALIGNMENT_FACTOR 0.05f

#define VELOCITY_LIMIT 350.0f
#define LOCATION_FACTOR 0.1f

BoidFlock::BoidFlock(TArray<AEnemyCharacter*> AIs, AWinningLocation *WinLoc, TArray<PathNode*> MapNodes){
	//Get every AI character
	this->AIs = AIs;
    this->WinLoc = WinLoc;
    this->MapNodes = MapNodes;
    
    //Calculate Boid Paths
    CalculateBoidsPaths();
}

BoidFlock::~BoidFlock(){
}


FVector BoidFlock::GetAICenter(){
    //Get average AI pos
    FVector AICenter;
    float pos = 0.0f;
    for(auto Boid : AIs){
        AICenter += Boid->GetActorLocation();
        pos += Boid->GetActorLocation().X;
    }
    
    AICenter /= AIs.Num();
    pos /= AIs.Num();
    
    return FVector(pos, AICenter.Y, AICenter.Z);
}


void BoidFlock::UpdateAIPositions(){
	FVector Cohesion,
		Allignment,
		Separation,
        GoalTendency;
    
    //Calculate A* path from Perceived Center to Goal location
    for(auto AI : AIs){
        Cohesion = CalculateBoidCohesion(AI);
        Allignment = CalculateBoidAlignment(AI);
        Separation = CalculateBoidSeparation(AI);
        GoalTendency = CalculateGoalTendency(AI, WinLoc->GetActorLocation());

        AI->GetCharacterMovement()->Velocity += Cohesion + Allignment + Separation + GoalTendency;
        LimitVelocity(AI);
        
        AEnemyAIController *Cont = Cast<AEnemyAIController>(AI->GetController());
        
        if(Cont){
            Cont->MoveToLocation(AStar::GetClosestNode(AI->GetCharacterMovement()->Velocity + AI->GetActorLocation(), MapNodes)->Position);
        }
    }
}


/**
 * Updates single AI position, based on created path
 */
FVector BoidFlock::UpdateAI(AEnemyCharacter *AI, const FVector &PosToGo){
    //Check if Boid in Crowd/Flock
    if(!AIs.Contains(AI)){
        UE_LOG(LogClass, Log, TEXT("BOID ERROR: AI not found in flock"));
        return FVector::ZeroVector;
    }
    
    FVector Cohesion = CalculateBoidCohesion(AI),
        Allignment = CalculateBoidAlignment(AI),
        Separation = CalculateBoidSeparation(AI),
        GoalTendency = CalculateGoalTendency(AI, PosToGo);
    
    AI->GetCharacterMovement()->Velocity += Cohesion + Allignment + Separation + GoalTendency;
    LimitVelocity(AI);
    
    return AStar::GetClosestNode(AI->GetCharacterMovement()->Velocity + AI->GetActorLocation(), MapNodes)->Position;
//    return FVector::ZeroVector;
}


void BoidFlock::CalculateBoidsPaths(){
    //Get average AI pos and calculate A* path
    TArray<FVector> PathLocations = AStar::GetAStarPath(GetAICenter(), WinLoc->GetActorLocation(), MapNodes);
    UE_LOG(LogClass, Log, TEXT("Path Size: %s"), *FString::FromInt(PathLocations.Num()));
    
    //Add Path to AI Controller
    for(auto Boid : AIs){
        AEnemyAIController *Cont = Cast<AEnemyAIController>(Boid->GetController());
        
        if(Cont){
            Cont->SetPathLocations(PathLocations);
        }
    }
}

/**
 *  ----------------------------------------------------------
 *	--------------------Boid Functions------------------------
 *	----------------------------------------------------------
 */
FVector BoidFlock::CalculateBoidCohesion(AEnemyCharacter *AI){
	FVector PerceivedCenter;

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
//    UE_LOG(LogClass, Log, TEXT("VELOCITY: %s"), *FString::SanitizeFloat(Vel.Size()));
	if(Vel.Size() > VELOCITY_LIMIT){
		Vel = (Vel / Vel.Size()) * VELOCITY_LIMIT;
	}
	
    AI->GetCharacterMovement()->Velocity = Vel;
}


void BoidFlock::CollisionDetect(AEnemyCharacter *AI){
//    float forward = AI->GetActorForwardVector().Size() / VELOCITY_LIMIT;
    FVector velocity = AI->GetCharacterMovement()->Velocity;
    float dynamicLen = velocity.Size() / VELOCITY_LIMIT;
    FVector ahead = AI->GetActorLocation() + velocity.GetSafeNormal() * dynamicLen;
    FVector ahead2 = ahead * 0.5f;
    
    
    
}


