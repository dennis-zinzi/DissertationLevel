// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class DISSERTATIONLEVEL_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

	public:
		// Sets default values for this character's properties
		AEnemyCharacter();

		// Called when the game starts or when spawned
		virtual void BeginPlay() override;
		
		// Called after character initialized
		virtual void PostInitializeComponents() override;

		// Called every frame
		virtual void Tick( float DeltaSeconds ) override;

		// Called to bind functionality to input
		virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

		UPROPERTY(EditAnywhere, Category = "AI|Behavior")
		class UBehaviorTree *AIBehavior;

		UPROPERTY(VisibleAnywhere, Category = "AI")
		class UPawnSensingComponent *PawnSensingComp;

	private:
		UFUNCTION()
		void OnPlayerCaught(APawn *Pawn);

	
};
