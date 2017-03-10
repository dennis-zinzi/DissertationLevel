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

	/** Generates items in a cone and places them in the environemtn */
	virtual void GenerateItems(FEnvQueryInstance &QueryInstance) const override;

	/* The distance between each point of the same Angle */
	UPROPERTY(EditAnywhere, Category = "EQS")
	float PointsDistance = 60.0f;

	UPROPERTY(EditAnywhere, Category = "EQS")
	float RangeDistance = 800.0f;

	UEnvQueryGenerator_AStarNodes();
};
