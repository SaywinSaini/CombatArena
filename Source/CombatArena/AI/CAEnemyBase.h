// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "CAEnemyBase.generated.h"

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
	
protected:
	
	virtual void BeginPlay() override;
	
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
};
