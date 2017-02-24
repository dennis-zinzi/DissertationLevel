// Fill out your copyright notice in the Description page of Project Settings.

#include "DissertationLevel.h"

#include "BehaviorTree/BehaviorTree.h"
#include "EnemyAIController.h"
#include "Perception/PawnSensingComponent.h"

#include "EnemyCharacter.h"


// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Initialize senses
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	PawnSensingComp->SetPeripheralVisionAngle(90.0f);
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	//Start Character at random position
	FVector randPos = GetActorLocation();
	randPos.X = FMath::RandRange(-1000.0f, 1000.0f);
	randPos.Y = FMath::RandRange(-1000.0f, 1000.0f);

	SetActorLocation(randPos);

	if(PawnSensingComp){
		PawnSensingComp->OnSeePawn.AddDynamic(this, &AEnemyCharacter::OnPlayerCaught);
	}
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

void AEnemyCharacter::OnPlayerCaught(APawn *Pawn){
	//Get reference to player controller
	AEnemyAIController *AIController = Cast<AEnemyAIController>(GetController());

	if(AIController){
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, "YOU'VE BEEN CAUGHT!");
		AIController->SetPlayerCaught(Pawn);
	}
}

