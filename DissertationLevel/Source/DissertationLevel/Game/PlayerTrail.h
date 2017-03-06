// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "PlayerTrail.generated.h"

UCLASS()
class DISSERTATIONLEVEL_API APlayerTrail : public AActor
{
	GENERATED_BODY()
	
	public:	
		// Sets default values for this actor's properties
		APlayerTrail();

		// Called when the game starts or when spawned
		virtual void BeginPlay() override;
	
		// Called every frame
		virtual void Tick(float DeltaSeconds) override;

		// Called after actor initialized
		virtual void PostInitializeComponents() override;

		//Return pickup's mesh
		FORCEINLINE UStaticMeshComponent* GetMesh() const{
			return TrailMesh;
		}

	private:
	//Function to call when player on top of winning spot
	UFUNCTION()
	void EnemySensed(UPrimitiveComponent *HitComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, FVector NormalImpulse, const FHitResult &Hit);


	//Static mesh to represent the winning spot on the level
	UPROPERTY(EditAnywhere, Category = "Game")
	class UStaticMeshComponent *TrailMesh;
};