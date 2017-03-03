// Fill out your copyright notice in the Description page of Project Settings.

#include "../DissertationLevel.h"

#include "../DissertationLevelCharacter.h"
#include "../AI/EnemyAIController.h"
#include "../AI/EnemyCharacter.h"

#include "PlayerTrail.h"


// Sets default values
APlayerTrail::APlayerTrail()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TrailMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TrailMesh"));
	const ConstructorHelpers::FObjectFinder<UStaticMesh> mesh_ref(TEXT("/Game/StarterContent/Shapes/Shape_Sphere"));
	TrailMesh->SetStaticMesh(mesh_ref.Object);
}

// Called when the game starts or when spawned
void APlayerTrail::BeginPlay()
{
	Super::BeginPlay();
	
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Black, "created trail spot");
}

// Called every frame
void APlayerTrail::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}


void APlayerTrail::PostInitializeComponents(){
	Super::PostInitializeComponents();

	//Add Hit listener
	TrailMesh->OnComponentHit.AddDynamic(this, &APlayerTrail::EnemySensed);
}


//Function to call when player on top of winning spot
void APlayerTrail::EnemySensed(UPrimitiveComponent *HitComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, FVector NormalImpulse, const FHitResult &Hit){
	ADissertationLevelCharacter *Player = Cast<ADissertationLevelCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());

	//Check Actor stepping on the spot is actually the player
	if(Player == OtherActor){
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, "CONGRATULATIONS!!!");
	}


	//Get the AI controller
	TArray<AActor*> AIs;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyAIController::StaticClass(), AIs);
	AEnemyAIController *ai = Cast<AEnemyAIController>(AIs[0]);

	if(ai){
		//Get AI character
		AEnemyCharacter *AIChar = Cast<AEnemyCharacter>(ai->GetPawn());

		if(AIChar){
			//Check Actor stepping on the spot is actually the AI
			if(AIChar == OtherActor){
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, "AI Sensed trail spot");
				Destroy();
			}
		}
	}

}