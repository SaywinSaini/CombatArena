
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CAEnemyAIController.generated.h"

class UAISenseConfig_Sight;
class UBehaviorTree;

UCLASS()
class COMBATARENA_API ACAEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	
	ACAEnemyAIController();

protected:
	
	virtual void OnPossess(APawn* InPawn) override;
	
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

private:
	
	// Sight sense configuration — range, angles, detection rules
	UPROPERTY()
	TObjectPtr<UAISenseConfig_Sight> SightConfig;
	
	// Behavior Tree asset assigned in Blueprint
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;
	
	FTimerHandle LostSightTimer;
};
