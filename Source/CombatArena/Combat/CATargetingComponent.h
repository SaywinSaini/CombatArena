// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CATargetingComponent.generated.h"

class APlayerController;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COMBATARENA_API UCATargetingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UCATargetingComponent();
	
	virtual void BeginPlay() override;
	
    AActor* FindBestTarget();
	
	void RotateTowardsTarget(AActor* Target,float DeltaTime);
	
	void ToggleTargetLock();
	
	bool IsTargetLocked() const { return bIsTargetLocked; }
	
	AActor* GetLockedTarget() const { return LockedTarget ; }
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	private:
	
	UPROPERTY()
	TObjectPtr<AActor> LockedTarget;
	
	bool bIsTargetLocked = false;
	
	UPROPERTY(EditDefaultsOnly,Category = "Combat")
	float TargetSearchRadius = 1500.0f;
	
	UPROPERTY(EditDefaultsOnly,Category = "Combat")
	float TargetingConeAngle = 60.0f;
	
	UPROPERTY(EditDefaultsOnly,Category = "Combat")
	float RotationInterpSpeed = 10.0f;
	
	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController;	
	
	UPROPERTY(EditDefaultsOnly,Category = "Targeting")
	float CameraInterpSpeed = 5.0f;
	
	UPROPERTY(EditDefaultsOnly,Category = "Targeting")
	float CameraVerticalOffset = -25.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float AimWeight = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float ProximityWeight = 0.3f;
	
};
