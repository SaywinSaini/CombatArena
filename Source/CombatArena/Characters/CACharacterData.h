// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CACharacterData.generated.h"

/**
 * Designer-tunable movement and combat parameters for the player character.
 */
class UAnimMontage;
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
	float RotationRate = 800.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float DodgeCooldown = 0.8f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float DodgeImpulse = 1500.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float BufferWindow = 0.2f;
	
	UPROPERTY(EditDefaultsOnly,Category= "Movement")
	float SpeedInterpSpeed = 10.0f;
	
	UPROPERTY(EditDefaultsOnly , Category= "Combat")
	float TraceRadius = 10.0f;
	
	UPROPERTY(EditDefaultsOnly , Category= "Combat")
	float TraceRange = 50.0f;
	
	UPROPERTY(EditDefaultsOnly ,Category= "Combat")
	FName WeaponSocketName = FName("hand_r");
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> BlockFlinchMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float BlockFlinchPlayRate = 0.5f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float BlockKnockbackStrength = 400.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Attributes")
	float MaxHealth = 100.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Attributes")
	float MaxStamina = 100.0f;
};
