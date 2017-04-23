// Fill out your copyright notice in the Description page of Project Settings.

#include "../DissertationLevel.h"

#include "BehaviorTree/BehaviorTree.h"
#include "EnemyAIController.h"
#include "Perception/PawnSensingComponent.h"
#include "../Game/WorldObject.h"

#include "EnemyCharacter.h"

//Sensing constants
#define PERIPHERAL_ANGLE 40.0f
#define SENSING_INTERVAL 0.25f
#define SIGHT_RADIUS 500.0f //1500.0f

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Initialize senses
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	PawnSensingComp->SetPeripheralVisionAngle(PERIPHERAL_ANGLE);
	PawnSensingComp->SensingInterval = SENSING_INTERVAL;
	PawnSensingComp->SightRadius = SIGHT_RADIUS;
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
//	//Start Character at random position
//	FVector randPos = GetActorLocation();
//	randPos.X = FMath::RandRange(-1000.0f, 1000.0f);
//	randPos.Y = FMath::RandRange(-1000.0f, 1000.0f);
//
//	SetActorLocation(randPos);
}

void AEnemyCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//Set Function to call when player in AI's view
    PawnSensingComp->OnSeePawn.AddDynamic(this, &AEnemyCharacter::OnSeenObstacle);//&AEnemyCharacter::OnPlayerCaught);
}

// Called every frame
void AEnemyCharacter::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

}

//void AEnemyCharacter::OnPlayerCaught(APawn *Pawn){
//	//Get reference to player controller
//	AEnemyAIController *AIController = Cast<AEnemyAIController>(GetController());
//
//	if(AIController){
//		AIController->SetPlayerCaught(Pawn);
//	}
//}


void AEnemyCharacter::OnSeenObstacle(APawn *Pawn){
    AWorldObject *Wobj = Cast<AWorldObject>(Pawn);
    
    if(Wobj){
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, "Obstacle in the vicinity!");
        
    }
}

