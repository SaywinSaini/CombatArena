// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "CABTTask_EnemyAttack.generated.h"

/**
 * 
 */
UCLASS()
class COMBATARENA_API UCABTTask_EnemyAttack : public UBTTaskNode
{
	GENERATED_BODY()
	
	public:
	
	UCABTTask_EnemyAttack();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
