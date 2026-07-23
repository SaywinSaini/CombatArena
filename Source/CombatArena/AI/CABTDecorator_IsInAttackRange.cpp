#include "CABTDecorator_IsInAttackRange.h"
#include "AIController.h"
#include "CAEnemyBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/CAEnemyData.h"

UCABTDecorator_IsInAttackRange::UCABTDecorator_IsInAttackRange()
{
	NodeName = TEXT("Is In Attack Range");

	INIT_DECORATOR_NODE_NOTIFY_FLAGS();

	bNotifyTick = false;
}

bool UCABTDecorator_IsInAttackRange::CalculateRawConditionValue(
	UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory) const
{
	ACAEnemyBase* Enemy = Cast<ACAEnemyBase>(OwnerComp.GetAIOwner()->GetPawn());
	if (!Enemy)
	{
		return false;
	}

	AActor* Player = Cast<AActor>(
		OwnerComp.GetBlackboardComponent()->GetValueAsObject(TEXT("PlayerActor")));

	if (!Player)
	{
		return false;
	}

	const float Range = Enemy->GetEnemyData()
		? Enemy->GetEnemyData()->AttackRange
		: 100.f;

	const float Distance = FVector::Dist(
		Enemy->GetActorLocation(),
		Player->GetActorLocation());

	const bool bInRange = Distance <= Range;

	return bInRange;
}
