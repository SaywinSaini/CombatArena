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
	
	FVector GetBestDirection(const AActor* Target);

protected:

	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Steering")
	int32 SampleCount = 16;
	
	UPROPERTY(EditDefaultsOnly, Category = "Steering")
	float ScoreInterval = 0.2f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Steering")
	float ProbeDistance = 100.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Steering")
	float RangeWeight = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Steering")
	float AngleWeight = 1.0f;
	
private:
	
	UPROPERTY()
	TObjectPtr<ACAEnemyBase> CachedOwner;
	
	FVector LastChosenDirection = FVector::ZeroVector;
	
	float LastScoreTime = -1000.0f;

};
