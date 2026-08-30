#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BillboardComponent.h"
#include "CASpawnPoint.generated.h"

/**
 * Marker placed in the level. The GameMode collects these at startup and
 * spawns wave enemies at their transforms.
 */

UCLASS()
class COMBATARENA_API ACASpawnPoint : public AActor
{
	GENERATED_BODY()

public:
	
	ACASpawnPoint();

private:
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBillboardComponent> Billboard;
};
