// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "AI/CAISlot.h"
#include "CAEnemyBase.generated.h"

class UCAHitDetectionComponent;
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
	
	void SetLastAttackTime(float Time) { LastAttackTime = Time; }
	float GetLastAttackTime() const { return LastAttackTime; }
	
	void SetLastEvadeTime(float Time) { LastEvadeTime = Time; }
	float GetLastEvadeTime() const { return LastEvadeTime; }
	
	void SetLastDashTime(float Time) { LastDashTime = Time; }
	float GetLastDashTime() const { return LastDashTime; }
	
	void ApplyDashInvulnerability(float Duration);
	
	void PlayHitReact(UAnimMontage* Montage, float PlayRate);
	bool IsReacting() const {return bIsReacting;}
	
	UCAHitDetectionComponent* GetHitDetectionComponent() const { return HitDetectionComponent; }

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
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCAHitDetectionComponent> HitDetectionComponent;
	
	EApproachSlot ClaimedSlot = EApproachSlot::None;
	
	UPROPERTY()
	TObjectPtr<ACASlotActor> SlotActor;
	
	UPROPERTY()
	TObjectPtr<APawn> CachedPlayer;
	
	EEnemyMovementState MovementState = EEnemyMovementState::Idle;
	
	float LastAttackTime = -1000.f;
	
	float LastEvadeTime  = -1000.f;
	
	float LastDashTime = -1000.f;
	
	FTimerHandle InvulnTimerHandle;
	
	bool bIsReacting = false;
	FTimerHandle HitReactTimerHandle;
};
