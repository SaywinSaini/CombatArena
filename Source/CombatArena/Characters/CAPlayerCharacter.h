// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "GenericTeamAgentInterface.h"
#include "CAPlayerCharacter.generated.h"

class UCAHitstopComponent;
class UCAMeleeAbility;
class UCATargetingComponent;
class UAIPerceptionStimuliSourceComponent;
class UCAHitDetectionComponent;
class UCACharacterMovementComponent;
class USpringArmComponent;
class UCameraComponent;
class UCACharacterData;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

UCLASS()
class COMBATARENA_API ACAPlayerCharacter : public ACharacter , public IAbilitySystemInterface , public IGenericTeamAgentInterface
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(VisibleAnywhere,Category = "Camera")
	TObjectPtr<UCameraComponent> Camera;
	
	UPROPERTY(VisibleAnywhere,Category = "Camera")
	TObjectPtr<USpringArmComponent> SpringArm;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> PlayerMappingContext;
	
	UPROPERTY(EditDefaultsOnly,Category = "Input")
	TObjectPtr<UInputAction> MoveAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> SprintAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> CrouchAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> DodgeAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> MeleeAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> RangedAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> BlockAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> TargetLockAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;
	
	virtual void BeginPlay() override;

	
public:
	
    ACAPlayerCharacter(const FObjectInitializer& ObjectInitializer);
	
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// Returns the AbilitySystemComponent for GAS to use externally
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override {return AbilitySystemComponent;}
	
	UCACharacterData* GetCharacterData() const { return CharacterData; }
	
	UCAHitDetectionComponent* GetHitDetectionComponent() const {return HitDetectionComponent;}
	
	UCATargetingComponent* GetTargetingComponent() const {return TargetingComponent;}
	
	UCAMeleeAbility* GetActiveMeleeAbility() const {return ActiveMeleeAbility;}
	
	virtual FGenericTeamId GetGenericTeamId() const override;
	
	virtual void FellOutOfWorld(const UDamageType& DmgType) override;
	
	virtual bool CanJumpInternal_Implementation() const override;
	
private: 
	
	UPROPERTY(VisibleAnywhere,Category = "Combat")
	TObjectPtr<UCAHitDetectionComponent> HitDetectionComponent;
	
	UPROPERTY(VisibleAnywhere, Category = "Combat")
	TObjectPtr<UCAHitstopComponent> HitstopComponent;
	
	UPROPERTY(VisibleAnywhere, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY(EditDefaultsOnly, Category = "Data")
	TObjectPtr<UCACharacterData> CharacterData;
	
	UPROPERTY(VisibleAnywhere, Category = "Combat")
	TObjectPtr<UCATargetingComponent> TargetingComponent;
	
	void Move(const FInputActionValue& Value);
	
	void Look(const FInputActionValue& Value);
    
	void StartCrouch();
	
	void StopCrouch();
	
	void ActivateMeleeAbility();
	
	void ActivateRangedAbility();
	
	void StartBlockAbility();
	
	void StopBlockAbility();
	
	void ToggleTargetLock();
	
	UPROPERTY(VisibleAnywhere, Category = "AI")
	TObjectPtr<UAIPerceptionStimuliSourceComponent> PerceptionStimuliSource;
	
	// Stores the ability spec handle assigned to the melee ability granted during initialization
	FGameplayAbilitySpecHandle MeleeAbilityHandle;
	
	UPROPERTY()
	TObjectPtr<UCAMeleeAbility> ActiveMeleeAbility;
	
	void OnAbilityActivated(UGameplayAbility* Ability);
	void OnAbilityEnded(UGameplayAbility* Ability);
};
