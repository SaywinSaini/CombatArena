// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CACharacterData.generated.h"

/**
 * Designer-tunable movement and combat parameters for the player character.
 */

UCLASS()
class COMBATARENA_API UCACharacterData : public UDataAsset
{
	GENERATED_BODY()
	
	public:
	
	UPROPERTY(EditDefaultsOnly,Category = "Movement" )
	float MovementSpeed = 400.0f;
	
	UPROPERTY(EditDefaultsOnly,Category = "Movement")
	float SprintSpeed = 1000.0f;
	
	UPROPERTY(EditDefaultsOnly,Category = "Movement")
	float JumpZVelocity = 700.0f;
	
	UPROPERTY(EditDefaultsOnly,Category = "Movement")
	float AirControl = 0.35f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float GravityScale = 1.75f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float RotationRate = 500.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float DodgeCooldown = 1.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float DodgeImpulse = 1500.0f;
	
	//How long a buffered input stays valid in seconds
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float BufferWindow = 0.2f;
	
	// Interpolation speed for transitioning between walk and sprint
	UPROPERTY(EditDefaultsOnly,Category= "Movement")
	float SpeedInterpSpeed = 10.0f;
	
	// Radius of the melee sphere trace used for hit detection.
	UPROPERTY(EditDefaultsOnly , Category= "Combat")
	float TraceRadius = 10.0f;
	
	// Forward distance of the melee sphere trace.
	UPROPERTY(EditDefaultsOnly , Category= "Combat")
	float TraceRange = 50.0f;
	
	//Socket used as origin for both melee traces and projectile spawning
	UPROPERTY(EditDefaultsOnly ,Category= "Combat")
	FName WeaponSocketName = FName("hand_r");
	
	UPROPERTY(EditDefaultsOnly, Category = "Attributes")
	float MaxHealth = 100.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Attributes")
	float MaxStamina = 100.0f;
};
