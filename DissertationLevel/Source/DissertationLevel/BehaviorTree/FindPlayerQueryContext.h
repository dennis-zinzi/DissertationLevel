// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EnvironmentQuery/EnvQueryContext.h"
#include "FindPlayerQueryContext.generated.h"

/**
 * 
 */
UCLASS()
class DISSERTATIONLEVEL_API UFindPlayerQueryContext : public UEnvQueryContext
{
	GENERATED_BODY()
	
	
	virtual void ProvideContext(FEnvQueryInstance &QueryInstance, FEnvQueryContextData &ContextData) const override;
	
};
