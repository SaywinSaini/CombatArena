// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CAGameplayAbility.h"
#include "CABlockAbility.generated.h"

/**
 * 
 */
UCLASS()
class COMBATARENA_API UCABlockAbility : public UCAGameplayAbility
{
	GENERATED_BODY()
	
	public:
	UCABlockAbility();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	// Ends the block ability when the player releases the block button
	void OnBlockReleased(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo);
	
	protected:
	//Damage reduction percentage while blocking
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	float BlockDamageReduction = 0.5f;
};

