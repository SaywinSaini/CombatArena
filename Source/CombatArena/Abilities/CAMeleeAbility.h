// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CAGameplayAbility.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "CAMeleeAbility.generated.h"


/**
 * 
 */
UCLASS()
class COMBATARENA_API UCAMeleeAbility : public UCAGameplayAbility
{
	GENERATED_BODY()
public:
	
	UCAMeleeAbility();
	
	void SetComboInputReceived();
	int32 GetComboIndex() const { return ComboIndex;}
	void AdvanceCombo();
	
protected:
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	// Called when montage finishes naturally
	UFUNCTION()
	void OnMontageCompleted();
	
	// Called when montage is interrupted or cancelled
	UFUNCTION()
	void OnMontageCancelled();
	
private:
	// Helper to avoid duplicating stop+end logic
	void StopAbility();
	
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	TObjectPtr<UAnimMontage> AttackMontage;
	
	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;
	
	int32 ComboIndex = 0;
	bool bComboInputReceived = false;
	
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	TArray<FName> ComboSections;
};
