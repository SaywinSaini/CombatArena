#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CAStunComponent.generated.h"

/**
 * Applies stun and drains it over time. Decay slows as the meter fills, so
 * early hits are easily shrugged off and sustained pressure is rewarded.
 */

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COMBATARENA_API UCAStunComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UCAStunComponent();

	void AddStun(float Amount);
	
	void ResetStun();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;
	
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Stun")
	float Tier1Threshold = 0.33f;

	UPROPERTY(EditDefaultsOnly, Category = "Stun")
	float Tier2Threshold = 0.66f;

	UPROPERTY(EditDefaultsOnly, Category = "Stun")
	float Tier1DecayRate = 3.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Stun")
	float Tier2DecayRate = 1.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Stun")
	float Tier3DecayRate = 1.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Stun")
	float DecayDelay = 2.0f;

private:
	float LastStunTime = -1000.f;
	
	
};