#pragma once

#include "CoreMinimal.h"
#include "CAEnemyRole.generated.h"

UENUM(BlueprintType)
enum class EEnemyRole : uint8
{
	// Commits to melee range directly in front of the player.
	Attacker     UMETA(DisplayName = "Attacker"),
	
	// Circles to the player's sides, pressuring without committing.
	Flanker      UMETA(DisplayName = "Flanker"),
	
	// Holds at range and closes only when an opening appears.
	Harasser     UMETA(DisplayName = "Harasser"),
};

USTRUCT(BlueprintType)
struct FEnemyRoleData
{

	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category = "Role")
	float MinRange = 100.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Role")
	float MaxRange = 150.0f;
	
	// 0 = directly in front of the player, 180 = directly behind.
	UPROPERTY(EditDefaultsOnly, Category = "Role")
	float MinAngle = 0.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Role")
	float MaxAngle = 45.0f;
};
