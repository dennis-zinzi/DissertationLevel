// Fill out your copyright notice in the Description page of Project Settings.

#include "../DissertationLevel.h"

#include "../DissertationLevelCharacter.h"
#include "../DissertationLevelGameMode.h"
#include "../AI/EnemyAIController.h"

#include "WinningLocation.h"


// Sets default values
AWinningLocation::AWinningLocation()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WinningSpotMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WinningSpotMesh"));

}

// Called when the game starts or when spawned
void AWinningLocation::BeginPlay()
{
	Super::BeginPlay();

	//Enable collisions
	SetActorEnableCollision(true);

//	//Start winning location at random position
//	FVector randPos = GetActorLocation();
//	randPos.X = FMath::RandRange(-1000.0f, 1000.0f);
//	randPos.Y = FMath::RandRange(-1000.0f, 1000.0f);
//
//	SetActorLocation(randPos);
}

// Called every frame
void AWinningLocation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AWinningLocation::PostInitializeComponents(){
	Super::PostInitializeComponents();

	//Add function to execute when player steps on winning spot
	WinningSpotMesh->OnComponentHit.AddDynamic(this, &AWinningLocation::PlayerOnTop);
}


//Function to call when player on top of winning spot
void AWinningLocation::PlayerOnTop(UPrimitiveComponent *HitComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, FVector NormalImpulse, const FHitResult &Hit){
	ADissertationLevelCharacter *Player = Cast<ADissertationLevelCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());

	//Check Actor stepping on the spot is actually the player
	if(Player == OtherActor){
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, "CONGRATULATIONS!!!");

		//Set Game to Won
		((ADissertationLevelGameMode *)GetWorld()->GetAuthGameMode())->SetCurrentState(EPlayState::EWin);
	}
    
    //Get AI Controller
    AEnemyAIController *AIController = Cast<AEnemyAIController>(OtherActor->GetInstigatorController());
    if(AIController){
        AIController->GetBlackboardComp()->SetValueAsBool(FName("Reached"), true);
    }
}
