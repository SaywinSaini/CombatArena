// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "AI/CAEnemyMovementState.h"
#include "GenericTeamAgentInterface.h"
#include "CAEnemyBase.generated.h"

class UCASteeringComponent;
class UCAHitDetectionComponent;
class APawn;
class UCAHitstopComponent;
class UCAEnemyData;
class UAbilitySystemComponent;
class UCAAttributeSet;

UCLASS()
class COMBATARENA_API ACAEnemyBase : public ACharacter, public IAbilitySystemInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	ACAEnemyBase();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override{ return AbilitySystemComponent; };
	
	virtual void Die();
	
	UAnimMontage* GetAttackMontage() const {return AttackMontage;};
	
	UCAEnemyData* GetEnemyData() const {return EnemyData; };
	
	UCAHitstopComponent* GetHitstopComponent() const {return HitstopComponent; };
	
	bool IsStrafing() const { return MovementState == EEnemyMovementState::Strafing; }
	
	void SetLastAttackTime(float Time) { LastAttackTime = Time; }
	float GetLastAttackTime() const { return LastAttackTime; }
	
	void SetLastEvadeTime(float Time) { LastEvadeTime = Time; }
	float GetLastEvadeTime() const { return LastEvadeTime; }
	
	void SetLastDashTime(float Time) { LastDashTime = Time; }
	float GetLastDashTime() const { return LastDashTime; }
	
	void ApplyDashInvulnerability(float Duration);
	
	void PlayHitReact(UAnimMontage* Montage, float PlayRate, FName Section = NAME_None);
	bool IsReacting() const {return bIsReacting;}
	
	UCAHitDetectionComponent* GetHitDetectionComponent() const { return HitDetectionComponent; }
	
	virtual FGenericTeamId GetGenericTeamId() const override;
	
	UCASteeringComponent* GetSteeringComponent() const { return SteeringComponent; }
	
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
	
	UPROPERTY()
	TObjectPtr<APawn> CachedPlayer;
	
	EEnemyMovementState MovementState = EEnemyMovementState::Idle;
	
	float LastAttackTime = -1000.f;
	
	float LastEvadeTime  = -1000.f;
	
	float LastDashTime = -1000.f;
	
	FTimerHandle InvulnTimerHandle;
	
	bool bIsReacting = false;
	FTimerHandle HitReactTimerHandle;
	
	UPROPERTY(VisibleAnywhere, Category = "AI")
	TObjectPtr<UCASteeringComponent> SteeringComponent;
};
