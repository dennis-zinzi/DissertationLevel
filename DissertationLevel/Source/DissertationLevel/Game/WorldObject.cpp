// Fill out your copyright notice in the Description page of Project Settings.

#include "DissertationLevel.h"

#include "../AI/EnemyAIController.h"

#include "WorldObject.h"

#define MOVE_DISTANCE_X 200.0f
#define MOVE_DISTANCE_Y 100.0f

// Sets default values
AWorldObject::AWorldObject()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ObjectMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TrailMesh"));
	const ConstructorHelpers::FObjectFinder<UStaticMesh> mesh_ref(TEXT("/Game/StarterContent/Shapes/Shape_Cube"));
	ObjectMesh->SetStaticMesh(mesh_ref.Object);
}

// Called when the game starts or when spawned
void AWorldObject::BeginPlay()
{
	Super::BeginPlay();
	
}


void AWorldObject::PostInitializeComponents(){
	Super::PostInitializeComponents();

	ObjectMesh->OnComponentHit.AddDynamic(this, &AWorldObject::ObjectHit);
}

// Called every frame
void AWorldObject::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}


void AWorldObject::ObjectHit(UPrimitiveComponent *HitComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, FVector NormalImpulse, const FHitResult &Hit){
	//float HitDirection = NormalImpulse.Rotation().Yaw;
	//float AIDir = OtherActor->GetActorForwardVector().Rotation().Yaw;
	AEnemyAIController *AI = Cast<AEnemyAIController>(OtherActor->GetInstigatorController());

	if(AI){
		FVector CurrentPos = OtherActor->GetActorLocation();
		float AIAbsDir = OtherActor->GetActorForwardVector().Rotation().Yaw;
		//float Dir = AIAbsDir < 0 ? (180.0f + AIAbsDir) / 2.0f : (180.0f - )
		if(AIAbsDir >= -90.0f &&  AIAbsDir < 0.0f){
			//Move Somewhere
			//OtherActor->SetActorLocation(CurrentPos + FVector(-MOVE_DISTANCE, 0.0f, 0.0f));
			AI->MoveToLocation(CurrentPos + FVector(-MOVE_DISTANCE_X, -MOVE_DISTANCE_Y, 0.0f), 10.0f);
			//OtherActor->SetActorRotation(FQuat(FVector(0,0,1.0f), 180 + AIAbsDir));
		}
		else if(AIAbsDir >= 0.0f &&  AIAbsDir < 90.0f){
			//OtherActor->SetActorLocation(CurrentPos + FVector(MOVE_DISTANCE, 0.0f, 0.0f));
			AI->MoveToLocation(CurrentPos + FVector(MOVE_DISTANCE_X, -MOVE_DISTANCE_Y, 0.0f), 10.0f);
			//OtherActor->SetActorRotation(FQuat(FVector(0, 0, 1.0f), 180 - AIAbsDir));
		}
		else if(AIAbsDir >= 90.0f && AIAbsDir < 180.0f){
			//OtherActor->SetActorLocation(CurrentPos + FVector(-MOVE_DISTANCE, 0.0f, 0.0f));
			AI->MoveToLocation(CurrentPos + FVector(-MOVE_DISTANCE_X, MOVE_DISTANCE_Y, 0.0f), 10.0f);
			//OtherActor->SetActorRotation(FQuat(FVector(0, 0, 1.0f), 180 - AIAbsDir));
		}
		else{
			//OtherActor->SetActorLocation(CurrentPos + FVector(MOVE_DISTANCE, 0.0f, 0.0f));
			AI->MoveToLocation(CurrentPos + FVector(MOVE_DISTANCE_X, MOVE_DISTANCE_Y, 0.0f), 10.0f);
			//OtherActor->SetActorRotation(FQuat(FVector(0, 0, 1.0f), 180 + AIAbsDir));
		}

		UE_LOG(LogClass, Log, TEXT("Hit with Rot: %s"), *FString::SanitizeFloat(AIAbsDir));
	}
	

	
}