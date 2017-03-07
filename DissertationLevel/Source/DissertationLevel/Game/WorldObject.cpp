// Fill out your copyright notice in the Description page of Project Settings.

#include "DissertationLevel.h"
#include "WorldObject.h"


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

// Called every frame
void AWorldObject::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

