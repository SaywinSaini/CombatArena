// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CAEnemyData.generated.h"

/**
 * Designer-tunable configuration data shared by enemy AI.
 */

UENUM(BlueprintType)
enum class EEnemyType : uint8
{
	Base                UMETA(DisplayName = "Base"),
	Aggressive          UMETA(DisplayName = "Aggressive"),
	Tank                UMETA(DisplayName = "Tank"),
};

UCLASS()
class COMBATARENA_API UCAEnemyData : public UDataAsset
{
	GENERATED_BODY()
	
public:
    
	UPROPERTY(EditDefaultsOnly,Category = "Combat")
	float AttackDamage = 20.0f;
	
	UPROPERTY(EditDefaultsOnly,Category = "Combat")
	float AttackRange = 150.0f;
	
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
	
	UPROPERTY(EditDefaultsOnly,Category = "AI")
	float SteeringEntryDistance = 600.0f;
	
	UPROPERTY(EditDefaultsOnly,Category = "AI")
	float SlotApproachDistance = 130.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float StrafeInwardBias = 0.35f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float PatrolSpeed = 300.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float ChaseSpeed = 600.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float StrafingSpeed = 200.0f;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float StrafingEntryDistance = 200.0f;
	
	UPROPERTY(EditDefaultsOnly,Category = "AI")
	EEnemyType EnemyType = EEnemyType::Base;
	
	// Spacing band: the enemy tries to hold this distance range from the player.
	UPROPERTY(EditDefaultsOnly, Category = "Spacing")
	float SpacingBandMin = 200.f;   // closer than this → back off

	UPROPERTY(EditDefaultsOnly, Category = "Spacing")
	float SpacingBandMax = 260.f;   // farther than this → approach

	UPROPERTY(EditDefaultsOnly, Category = "Spacing")
	float SpacingSpeed = 300.f;     // how fast it adjusts spacing
	
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float EvadeCooldown = 1.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> HitReactMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float HitReactPlayRate = 1.5f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	FName WeaponSocketName;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float TraceRange = 50.f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float TraceRadius = 10.f;
};
