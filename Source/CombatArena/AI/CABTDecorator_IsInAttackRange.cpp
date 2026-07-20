// Fill out your copyright notice in the Description page of Project Settings.


#include "CABTDecorator_IsInAttackRange.h"

#include "AIController.h"
#include "CAEnemyBase.h"
#include "Characters/CAEnemyData.h"
#include "AI/CASlotActor.h"
#include "BehaviorTree/BlackboardComponent.h"


UCABTDecorator_IsInAttackRange::UCABTDecorator_IsInAttackRange()
{
	bNotifyTick = true;
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
	
	// Attack range is measured to the PLAYER (what we hit), never to the slot
	// (our own standing position). This is the reference point that decides
	// whether a swing can actually land.
	const float Range = Enemy->GetEnemyData() ? Enemy->GetEnemyData()->AttackRange : 100.0f;
	const float Distance = FVector::Dist(Enemy->GetActorLocation(), Player->GetActorLocation());

	const bool bInRange = Distance <= Range;
	return IsInversed() ? !bInRange : bInRange;
}

void UCABTDecorator_IsInAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
}
