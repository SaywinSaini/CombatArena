#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CAHitDetectionComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COMBATARENA_API UCAHitDetectionComponent : public UActorComponent
{
	GENERATED_BODY()
	
protected:
	
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly , Category= "Trace")
	float TraceRadius = 50.0f;
	
	UPROPERTY(EditDefaultsOnly , Category= "Trace")
	float TraceRange = 150.0f;
	
	// Tracks actors already hit this swing to prevent multiple hits per attack
	UPROPERTY()
	TSet<TObjectPtr<AActor>> HitActors;
	
	bool bIsTracing = false;

public:
	
	UCAHitDetectionComponent();
	
	void StartTrace();
	
	void StopTrace();
	
	void PerformTrace();
	
};
