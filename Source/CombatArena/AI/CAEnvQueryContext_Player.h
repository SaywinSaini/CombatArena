#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "CAEnvQueryContext_Player.generated.h"

UCLASS()
class COMBATARENA_API UCAEnvQueryContext_Player : public UEnvQueryContext
{
	GENERATED_BODY()

protected:
	
	// Returns player location as EQS context
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance,
		FEnvQueryContextData& ContextData) const override;
};