// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "CABTTask_FindSearchLocation.generated.h"

UCLASS()
class COMBATARENA_API UCABTTask_FindSearchLocation : public UBTTaskNode
{
	GENERATED_BODY()
	
	public:
	
	UCABTTask_FindSearchLocation();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp,uint8* NodeMemory) override;
	
private:
	
	//Search radius around last known player position
	UPROPERTY(EditAnywhere,Category = "AI")
	float SearchRadius = 600.0f;
	
	// Blackboard key storing search destination
	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector SearchLocationKey;
};
