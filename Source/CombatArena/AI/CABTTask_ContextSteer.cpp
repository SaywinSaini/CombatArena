#include "CABTTask_ContextSteer.h"

#include "AIController.h"
#include "CAEnemyBase.h"
#include "CASteeringComponent.h"
#include "Characters/CAEnemyData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UCABTTask_ContextSteer::UCABTTask_ContextSteer()
{
	NodeName = TEXT("Context Steer");
	bNotifyTick = true;
}

EBTNodeResult::Type UCABTTask_ContextSteer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::InProgress;
}

void UCABTTask_ContextSteer::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC) { FinishLatentTask(OwnerComp, EBTNodeResult::Failed); return; }

	ACAEnemyBase* Enemy = Cast<ACAEnemyBase>(AIC->GetPawn());
	if (!Enemy || !Enemy->GetEnemyData()) { FinishLatentTask(OwnerComp, EBTNodeResult::Failed); return; }

	if (Enemy->IsReacting()) return;

	UCASteeringComponent* Steering = Enemy->GetSteeringComponent();
	if (!Steering) { FinishLatentTask(OwnerComp, EBTNodeResult::Failed); return; }

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) { FinishLatentTask(OwnerComp, EBTNodeResult::Failed); return; }

	AActor* Player = Cast<AActor>(BB->GetValueAsObject(TEXT("PlayerActor")));
	if (!Player) { FinishLatentTask(OwnerComp, EBTNodeResult::Failed); return; }

	const UCAEnemyData* Data = Enemy->GetEnemyData();

	// Close to attack range when ready to swing, hold the standoff band otherwise.
	const bool bCooldownReady = BB->GetValueAsBool(TEXT("bAttackCooldownReady"));
	const float BandMin = bCooldownReady ? Data->AttackBandMin : Data->SpacingBandMin;
	const float BandMax = bCooldownReady ? Data->AttackBandMax : Data->SpacingBandMax;

	const float DistToPlayer = FVector::Dist2D(Enemy->GetActorLocation(), Player->GetActorLocation());

	// Run when far, walk when settling, so small corrections aren't made at sprint speed.
	const bool bFarFromBand = DistToPlayer > BandMax + Data->SpacingApproachRange;
	Enemy->GetCharacterMovement()->MaxWalkSpeed = bFarFromBand ? Enemy->GetChaseSpeed() : Data->SpacingSpeed;

	const FVector Direction = Steering->GetBestDirection(Player, BandMin, BandMax);
	if (!Direction.IsNearlyZero())
	{
		Enemy->AddMovementInput(Direction, MovementScale);
	}
}