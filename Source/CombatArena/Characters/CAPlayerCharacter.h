// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "CAPlayerCharacter.generated.h"

class UCACharacterMovementComponent;
class USpringArmComponent;
class UCameraComponent;
class UCACharacterData;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;
UCLASS()
class COMBATARENA_API ACAPlayerCharacter : public ACharacter , public IAbilitySystemInterface
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
	
	virtual void BeginPlay() override;


public:
	
    ACAPlayerCharacter(const FObjectInitializer& ObjectInitializer);
	
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// Returns the AbilitySystemComponent for GAS to use externally
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override {return AbilitySystemComponent;}
	
	UCACharacterData* GetCharacterData() const { return CharacterData; }
private: 
	
	UPROPERTY(VisibleAnywhere, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY(EditDefaultsOnly, Category = "Data")
	TObjectPtr<UCACharacterData> CharacterData;
	
	void Move(const FInputActionValue& Value);
	
	void Look(const FInputActionValue& Value);
    
	void StartCrouch();
	
	void StopCrouch();
};
