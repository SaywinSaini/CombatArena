// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "CAGameplayAbility.generated.h"

UENUM(BlueprintType)
enum class ECAAbilityActivationPolicy : uint8
{
	OnInputTriggered,
	OnGranted
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
