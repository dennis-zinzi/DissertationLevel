// Fill out your copyright notice in the Description page of Project Settings.

#include "DissertationLevel.h"
#include "EnemyCharacter.h"


// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

