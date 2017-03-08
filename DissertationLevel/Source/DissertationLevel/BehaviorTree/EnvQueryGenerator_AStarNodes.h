// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EnvironmentQuery/Generators/EnvQueryGenerator_ProjectedPoints.h"
#include "EnvQueryGenerator_AStarNodes.generated.h"

/**
 * 
 */
UCLASS()
class DISSERTATIONLEVEL_API UEnvQueryGenerator_AStarNodes : public UEnvQueryGenerator_ProjectedPoints
{
	GENERATED_BODY()

	//virtual void GenerateItems(FEnvQueryInstance &QueryInstance) const override;
	//
	//private:
		UPROPERTY(EditAnywhere, Category = "EQS")
		float PointsDistance = 50.0f;

		UPROPERTY(EditAnywhere, Category = "EQS")
		float PointsSeperation = 25.0f;

		UPROPERTY(EditAnywhere, Category = "EQS")
		float Bounds = 100.0f;

		UPROPERTY(EditAnywhere, Category = "EQS")
		float HalfLen = 150.0f;

	/** Generates items in a cone and places them in the environemtn */
	virtual void GenerateItems(FEnvQueryInstance& QueryInstance) const override;

	///* The distance between each point of the same Angle */
	//UPROPERTY(EditAnywhere, Category = "EQS")
	//float PointsDistance = 50.0f;

	///* The maximum degrees of the generated cone */
	//UPROPERTY(EditAnywhere, Category = "EQS")
	//float ConeDegrees = 60.f;

	///* Angle Step is the step that the angles increase. A small value means that more item will get generated */
	//UPROPERTY(EditAnywhere, Category = "EQS")
	//float AngleStep = 10.0f;

	///* The radius of our cone */
	//UPROPERTY(EditAnywhere, Category = "EQS")
	//float ConeRadius = 150.f;

	UEnvQueryGenerator_AStarNodes();
};
