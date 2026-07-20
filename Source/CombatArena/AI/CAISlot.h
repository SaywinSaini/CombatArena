#pragma once

#include "CoreMinimal.h"
#include "CAISlot.generated.h"

UENUM(BlueprintType)
enum class EApproachSlot : uint8
{
	Front           UMETA(DisplayName = "Front"),
	FrontRight      UMETA(DisplayName = "FrontRight"),
	Right           UMETA(DisplayName = "Right"),
	BackRight       UMETA(DisplayName = "BackRight"),
	Back            UMETA(DisplayName = "Back"),
	BackLeft        UMETA(DisplayName = "BackLeft"),
	Left            UMETA(DisplayName = "Left"),
	FrontLeft       UMETA(DisplayName = "FrontLeft"),
	None            UMETA(DisplayName = "None"),
};

UENUM(BlueprintType)
enum class EEnemyMovementState : uint8
{
	Idle        UMETA(DisplayName = "Idle"),
	Patrol      UMETA(DisplayName = "Patrol"),
	Chase       UMETA(DisplayName = "Chase"),
	Strafing    UMETA(DisplayName = "Strafing"),
	Attack      UMETA(DisplayName = "Attack"),
};