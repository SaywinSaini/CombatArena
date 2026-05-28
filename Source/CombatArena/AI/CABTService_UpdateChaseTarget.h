// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "CABTService_UpdateChaseTarget.generated.h"

UCLASS()
class COMBATARENA_API UCABTService_UpdateChaseTarget : public UBTService
{
	GENERATED_BODY()
	
	public:
	UCABTService_UpdateChaseTarget();
	
protected:
	
	// Called on interval while the node this service is attached to is active
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
	private:
	
	// Blackboard key to write EQS result into — read by MoveTo task
	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector ChaseTargetKey;
	
};
