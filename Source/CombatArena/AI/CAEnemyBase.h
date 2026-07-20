// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "AI/CAISlot.h"
#include "CAEnemyBase.generated.h"

class ACASlotActor;
class APawn;
class UCAHitstopComponent;
class UCAEnemyData;
class UAbilitySystemComponent;
class UCAAttributeSet;

UCLASS()
class COMBATARENA_API ACAEnemyBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ACAEnemyBase();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override{ return AbilitySystemComponent; };
	
	virtual void Die();
	
	UAnimMontage* GetAttackMontage() const {return AttackMontage;};
	
	UCAEnemyData* GetEnemyData() const {return EnemyData; };
	
	UCAHitstopComponent* GetHitstopComponent() const {return HitstopComponent; };
	
	EApproachSlot GetClaimedSlot() const { return ClaimedSlot; }
	
	ACASlotActor* GetSlotActor() const {return SlotActor;}
	
	bool IsStrafing() const { return MovementState == EEnemyMovementState::Strafing; }

	
protected:
	
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaSeconds) override;
	
	UPROPERTY(EditDefaultsOnly,Category = "Combat")
	TObjectPtr<UAnimMontage> AttackMontage;
	
private:
	
	// Handles attribute storage and GAS damage application
    UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	
	// Tracks Health and MaxHealth for this enemy
	UPROPERTY()
	TObjectPtr<UCAAttributeSet> AttributeSet;

    UPROPERTY(EditAnywhere,Category = "Data")
	TObjectPtr<UCAEnemyData> EnemyData;
	
	UPROPERTY(VisibleAnywhere,Category = "Combat")
	TObjectPtr<UCAHitstopComponent> HitstopComponent;
	
	EApproachSlot ClaimedSlot = EApproachSlot::None;
	
	UPROPERTY()
	TObjectPtr<ACASlotActor> SlotActor;
	
	UPROPERTY()
	TObjectPtr<APawn> CachedPlayer;
	
	EEnemyMovementState MovementState = EEnemyMovementState::Idle;
};
