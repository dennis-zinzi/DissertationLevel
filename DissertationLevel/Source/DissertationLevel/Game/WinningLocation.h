// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "WinningLocation.generated.h"

UCLASS()
class DISSERTATIONLEVEL_API AWinningLocation : public AActor
{
	GENERATED_BODY()
	
	public:	
		// Sets default values for this actor's properties
		AWinningLocation();

		// Called when the game starts or when spawned
		virtual void BeginPlay() override;
	
		// Called every frame
		virtual void Tick( float DeltaSeconds ) override;

		// Called after actor initialized
		virtual void PostInitializeComponents() override;

		//Return pickup's mesh
		FORCEINLINE class UStaticMeshComponent* GetMesh() const{
			return WinningSpotMesh;
		}

	private:
		//Function to call when player on top of winning spot
		UFUNCTION()
		void PlayerOnTop(UPrimitiveComponent *HitComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, FVector NormalImpulse, const FHitResult &Hit);


		//Static mesh to represent the winning spot on the level
		UPROPERTY(EditAnywhere, Category = "Game")
		class UStaticMeshComponent *WinningSpotMesh;
};
