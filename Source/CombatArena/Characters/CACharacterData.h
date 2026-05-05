// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CACharacterData.generated.h"


UCLASS()
class COMBATARENA_API UCACharacterData : public UDataAsset
{
	GENERATED_BODY()
	
	public:
	
	//Movement
	UPROPERTY(EditDefaultsOnly,Category = "Movement" )
	float MovementSpeed = 600.0f;
	
	UPROPERTY(EditDefaultsOnly,Category = "Movement")
	float SprintSpeed = 1000.0f;
	
	UPROPERTY(EditDefaultsOnly,Category = "Movement")
	float JumpZVelocity = 700.0f;
	
	UPROPERTY(EditDefaultsOnly,Category = "Movement")
	float AirControl = 0.35f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float GravityScale = 1.75f;
    // Yaw Rotation
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float RotationRate = 500.f;
	
	
};
