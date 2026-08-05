#pragma once

#include "CoreMinimal.h"
#include "CAEnemyMovementState.generated.h"

UENUM(BlueprintType)
enum class EEnemyMovementState : uint8
{
	Idle        UMETA(DisplayName = "Idle"),
	Patrol      UMETA(DisplayName = "Patrol"),
	Chase       UMETA(DisplayName = "Chase"),
	Strafing    UMETA(DisplayName = "Strafing"),
	Attack      UMETA(DisplayName = "Attack"),
};