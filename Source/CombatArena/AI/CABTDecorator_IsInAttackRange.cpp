// Fill out your copyright notice in the Description page of Project Settings.


#include "CABTDecorator_IsInAttackRange.h"

#include "AIController.h"
#include "CAEnemyBase.h"
#include "BehaviorTree/BlackboardComponent.h"


UCABTDecorator_IsInAttackRange::UCABTDecorator_IsInAttackRange()
{
	NodeName = TEXT("Is In Attack Range");
}

bool UCABTDecorator_IsInAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory) const
{
	// Get the enemy pawn
	ACAEnemyBase* Enemy = Cast<ACAEnemyBase>(OwnerComp.GetAIOwner()->GetPawn());
	if (!Enemy) return false;
	
	// Get player actor from Blackboard
	AActor* Player = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TEXT("PlayerActor")));
	if (!Player) return false;
	
	// Check if player is within attack range
	float Distance = FVector::Dist(Enemy->GetActorLocation(), Player->GetActorLocation());
	return Distance <= AttackRange;
}
