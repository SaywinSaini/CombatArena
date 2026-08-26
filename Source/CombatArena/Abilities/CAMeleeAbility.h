// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CAGameplayAbility.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "CAMeleeAbility.generated.h"


/**
 * Gameplay ability that executes a melee combo sequence using
 * montage sections and buffered player input.
 */
UCLASS()
class COMBATARENA_API UCAMeleeAbility : public UCAGameplayAbility
{
	GENERATED_BODY()
public:
	
	UCAMeleeAbility();
	
	// Records player input during a combo window.
	void SetComboInputReceived();
	int32 GetComboIndex() const { return ComboIndex;}
	
	// Advances to the next combo section if buffered input exists.
	void AdvanceCombo();
	
protected:
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	UFUNCTION()
	void OnMontageCompleted();
	
	UFUNCTION()
	void OnMontageCancelled();
	
private:
	
	void StopAbility();
	
	// Montage used to play the melee attack sequence.
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	TObjectPtr<UAnimMontage> AttackMontage;
	
	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;
	
	int32 ComboIndex = 0;
	bool bComboInputReceived = false;
	
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	TArray<FName> ComboSections;
};
