// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CAEnemyData.generated.h"

/**
 * Designer-tunable configuration data shared by enemy AI.
 */

UCLASS()
class COMBATARENA_API UCAEnemyData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditDefaultsOnly,Category = "Movement")
	float MovementSpeed = 600.0f;
    
	UPROPERTY(EditDefaultsOnly,Category = "Combat")
	float AttackDamage = 20.0f;
	
	UPROPERTY(EditDefaultsOnly,Category = "Combat")
	float AttackRange = 200.0f;
	
	UPROPERTY(EditDefaultsOnly,Category = "Combat")
	float AttackCooldown = 2.0f;
	
	UPROPERTY(EditDefaultsOnly,Category = "Perception")
    float SightRadius = 800.0f;
	
	UPROPERTY(EditDefaultsOnly,Category = "Perception")
	float LoseSightRadius = 1000.0f;
	
	UPROPERTY(EditDefaultsOnly,Category = "Perception")
	float PeripheralVisionAngle = 120.0f;
	
	UPROPERTY(EditDefaultsOnly,Category = "Patrol")
	float PatrolRadius = 1000.0f;
	
	UPROPERTY(EditDefaultsOnly,Category = "Health")
	float MaxHealth = 100.0f;
	
	UPROPERTY(EditDefaultsOnly,Category = "AI")
	float LostSightMemoryDuration = 5.0f;
};
