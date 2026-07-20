// Fill out your copyright notice in the Description page of Project Settings.
#include "CABTTask_MoveToSlot.h"
#include "AIController.h"
#include "CAEnemyBase.h"
#include "Characters/CAEnemyData.h"
#include "AI/CASlotActor.h"
#include "BehaviorTree/BlackboardComponent.h"

UCABTTask_MoveToSlot::UCABTTask_MoveToSlot()
{
    NodeName = TEXT("Move To Slot");
    bNotifyTick = true;
}

EBTNodeResult::Type UCABTTask_MoveToSlot::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIC = OwnerComp.GetAIOwner();
    if (!AIC) return EBTNodeResult::Failed;

    ACAEnemyBase* Enemy = Cast<ACAEnemyBase>(AIC->GetPawn());
    if (!Enemy || !Enemy->GetEnemyData()) return EBTNodeResult::Failed;

    ACASlotActor* SlotActor = Enemy->GetSlotActor();
    if (!SlotActor) return EBTNodeResult::Failed;

    // Kick off pathing to the slot. MoveToActor tracks the actor as it moves, so a slot
    // pinned to the player is followed natively — no manual re-issue needed.
    AIC->MoveToActor(SlotActor, AcceptanceRadius);

    return EBTNodeResult::InProgress;
}

void UCABTTask_MoveToSlot::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    AAIController* AIC = OwnerComp.GetAIOwner();
    if (!AIC) { FinishLatentTask(OwnerComp, EBTNodeResult::Failed); return; }

    ACAEnemyBase* Enemy = Cast<ACAEnemyBase>(AIC->GetPawn());
    if (!Enemy || !Enemy->GetEnemyData()) { FinishLatentTask(OwnerComp, EBTNodeResult::Failed); return; }

    ACASlotActor* SlotActor = Enemy->GetSlotActor();
    if (!SlotActor) { FinishLatentTask(OwnerComp, EBTNodeResult::Failed); return; }

    APawn* Player = nullptr;
    if (UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent())
    {
        Player = Cast<APawn>(BB->GetValueAsObject(TEXT("PlayerActor")));
    }
    if (!Player) { FinishLatentTask(OwnerComp, EBTNodeResult::Failed); return; }

    const float DistToPlayer = FVector::Dist(Enemy->GetActorLocation(), Player->GetActorLocation());

    // Success condition is measured to the PLAYER (the thing we attack), not the slot.
    // Once in range, stop moving and hand off to the Attack branch.
    if (DistToPlayer <= Enemy->GetEnemyData()->AttackRange)
    {
        AIC->StopMovement();
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        return;
    }

    // Keep chasing the (moving) slot. Re-issuing is cheap and covers the case where the
    // player moved enough that the previous MoveTo already reported done short of us.
    AIC->MoveToActor(SlotActor, AcceptanceRadius);
}