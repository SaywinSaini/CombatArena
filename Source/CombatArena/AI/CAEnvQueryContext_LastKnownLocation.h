// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "CAEnvQueryContext_LastKnownLocation.generated.h"

/**
 * 
 */
UCLASS()
class COMBATARENA_API UCAEnvQueryContext_LastKnownLocation : public UEnvQueryContext
{
	GENERATED_BODY()
	
	protected:
	
	//Provides last remembered player position from blackboard
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
};
