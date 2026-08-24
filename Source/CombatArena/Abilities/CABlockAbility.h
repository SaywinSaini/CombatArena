// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CAGameplayAbility.h"
#include "CABlockAbility.generated.h"

/**
 * Gameplay ability associated with blocking.
 */
UCLASS()
class COMBATARENA_API UCABlockAbility : public UCAGameplayAbility
{
	GENERATED_BODY()
	
	public:
	UCABlockAbility();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};

