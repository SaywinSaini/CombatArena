// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CAGameplayAbility.h"
#include "CAMeleeAbility.generated.h"

/**
 * 
 */
UCLASS()
class COMBATARENA_API UCAMeleeAbility : public UCAGameplayAbility
{
	GENERATED_BODY()
	
	protected:
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	TObjectPtr<UAnimMontage> AttackMontage;
	
public:
	UCAMeleeAbility();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
