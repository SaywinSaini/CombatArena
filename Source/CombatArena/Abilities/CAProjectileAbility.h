// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CAGameplayAbility.h"
#include "CAProjectileAbility.generated.h"
/**
 * Gameplay ability responsible for spawning a projectile actor
 * in front of the owning character.
 */
UCLASS()
class COMBATARENA_API UCAProjectileAbility : public UCAGameplayAbility
{
	GENERATED_BODY()
	
	public:
	UCAProjectileAbility();
	
	/**
	* Spawns the configured projectile actor and immediately ends the ability.
	*/
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
protected:
	
	// Projectile actor class spawned when the ability is activated.
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<AActor> ProjectileClass;
	
	// Distance in front of the character used as the projectile spawn position.
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	float SpawnOffset = 100.f;
};
