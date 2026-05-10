// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CAGameplayAbility.h"
#include "CAProjectileAbility.generated.h"

/**
 * 
 */
UCLASS()
class COMBATARENA_API UCAProjectileAbility : public UCAGameplayAbility
{
	GENERATED_BODY()
	
	public:
	UCAProjectileAbility();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
protected:
	
	//Projectile class to spawn
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<AActor> ProjectileClass;
	
	//Spawn offset from characrter location
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	float SpawnOffset = 100.f;
};
