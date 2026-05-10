// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Abilities/GameplayAbility.h"
#include "CAGameplayAbility.generated.h"


UENUM(BlueprintType)
enum class ECAAbilityActivationPolicy : uint8
{
	OnInputTriggered,
	OnGranted
};

USTRUCT(BlueprintType)
struct  FCAAbilityData : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	float Damage = 20.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	float StaminaCost = 10.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	float Cooldown = 1.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	float Range = 200.0f;
};

UCLASS()
class COMBATARENA_API UCAGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
	public:
	
	// Returns the activation policy for this ability
	ECAAbilityActivationPolicy GetActivationPolicy() const
	{
		return ActivationPolicy;
	}
	
	protected:
	
	// Defines when this ability activates
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	ECAAbilityActivationPolicy ActivationPolicy = ECAAbilityActivationPolicy::OnInputTriggered;
};
