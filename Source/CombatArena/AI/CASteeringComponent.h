// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CASteeringComponent.generated.h"


class ACAEnemyBase;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COMBATARENA_API UCASteeringComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UCASteeringComponent();
	
	FVector GetBestDirection(const AActor* Target, float BandMin, float BandMax);

protected:

	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Steering")
	int32 SampleCount = 16;
	
	UPROPERTY(EditDefaultsOnly, Category = "Steering")
	float ScoreInterval = 0.2f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Steering")
	float ProbeDistance = 60.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Steering")
	float RangeWeight = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Steering")
	float SwitchMargin = 15.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Steering")
	float DirectionInterpSpeed = 240.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Steering")
    float AvoidanceWeight = 3.0f;
    
    UPROPERTY(EditDefaultsOnly, Category = "Steering")
    float AvoidanceRadius = 120.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Steering")
	float FormationWeight = 3.0f;
	
private:
	
	UPROPERTY()
	TObjectPtr<ACAEnemyBase> CachedOwner;
	
	FVector LastChosenDirection = FVector::ZeroVector;
	
	float LastScoreTime = -1000.0f;

};
