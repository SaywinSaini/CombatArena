// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "CABTTask_FindPatrolLocation.generated.h"


UCLASS()
class COMBATARENA_API UCABTTask_FindPatrolLocation : public UBTTaskNode
{
	GENERATED_BODY()
	
	public:
	
	UCABTTask_FindPatrolLocation();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	private:
	
	// Search radius for random reachable navigation point — tunable per enemy type
	UPROPERTY(EditAnywhere,Category = "AI")
	float PatrolRadius = 1000.0f;
	
	//// Blackboard key written with the chosen patrol destination each cycle
	UPROPERTY(EditAnywhere,Category = "AI")
	FBlackboardKeySelector PatrolLocationKey;
};
