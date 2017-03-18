// Fill out your copyright notice in the Description page of Project Settings.

#include "DissertationLevel.h"

#include "../AI/EnemyAIController.h"

#include "BoidFlock.h"

//Percentage
#define COHESION_FACTOR 0.1f
#define OVERLAP_DISTANCE 100.0f
#define ALIGNMENT_FACTOR 0.05f

#define VELOCITY_LIMIT 350.0f

BoidFlock::BoidFlock(TArray<AEnemyCharacter*> AIs, ADissertationLevelCharacter *Player){
	//Get every AI character
	this->AIs = AIs;
    this->Player = Player;
}

BoidFlock::~BoidFlock(){
}

void BoidFlock::UpdateAIPositions(){
	FVector Cohesion,
		Allignment,
		Separation;

	for(auto AI : AIs){
		Cohesion = CalculateBoidCohesion(AI);
		Allignment = CalculateBoidAlignment(AI);
		Separation = CalculateBoidSeparation(AI);

        AI->GetCharacterMovement()->Velocity += Cohesion + Allignment + Separation;
        LimitVelocity(AI);
        
        AEnemyAIController *Cont = Cast<AEnemyAIController>(AI->GetController());
        
        if(Cont){
            Cont->MoveToLocation(AI->GetCharacterMovement()->Velocity + AI->GetActorLocation());
        }
        
		
        
        
        
//		AI->SetActorLocation(AI->GetCharacterMovement()->Velocity + AI->GetActorLocation());
	}
}

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
//	return (Player->GetActorLocation() - AI->GetActorLocation()) * COHESION_FACTOR;
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


void BoidFlock::LimitVelocity(AEnemyCharacter *AI){
	FVector Vel = AI->GetCharacterMovement()->Velocity;
    UE_LOG(LogClass, Log, TEXT("VELOCITY: %s"), *FString::SanitizeFloat(Vel.Size()));
	if(Vel.Size() > VELOCITY_LIMIT){
		Vel = (Vel / Vel.Size()) * VELOCITY_LIMIT;
	}
	
    AI->GetCharacterMovement()->Velocity = Vel;
}
