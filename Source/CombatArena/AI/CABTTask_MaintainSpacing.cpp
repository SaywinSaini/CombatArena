#include "CABTTask_MaintainSpacing.h"
#include "AIController.h"
#include "CAEnemyBase.h"
#include "Characters/CAEnemyData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UCABTTask_MaintainSpacing::UCABTTask_MaintainSpacing()
{
    NodeName = TEXT("Maintain Spacing");
    bNotifyTick = true;
}

EBTNodeResult::Type UCABTTask_MaintainSpacing::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIC = OwnerComp.GetAIOwner();
    if (AIC)
    {
        if (ACAEnemyBase* Enemy = Cast<ACAEnemyBase>(AIC->GetPawn()))
        {
            if (Enemy->GetEnemyData())
            {
                Enemy->GetCharacterMovement()->MaxWalkSpeed = Enemy->GetEnemyData()->SpacingSpeed;
            }
        }
    }
    return EBTNodeResult::InProgress;
}

void UCABTTask_MaintainSpacing::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    AAIController* AIC = OwnerComp.GetAIOwner();
    if (!AIC) { FinishLatentTask(OwnerComp, EBTNodeResult::Failed); return; }

    ACAEnemyBase* Enemy = Cast<ACAEnemyBase>(AIC->GetPawn());
    if (!Enemy || !Enemy->GetEnemyData()) { FinishLatentTask(OwnerComp, EBTNodeResult::Failed); return; }

    // Get the player from the Blackboard
    AActor* Player = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TEXT("PlayerActor")));
    if (!Player) { FinishLatentTask(OwnerComp, EBTNodeResult::Failed); return; }

    const FVector EnemyLoc  = Enemy->GetActorLocation();
    const FVector PlayerLoc = Player->GetActorLocation();
    const float   DistToPlayer = FVector::Dist(EnemyLoc, PlayerLoc);

    const float BandMin = Enemy->GetEnemyData()->SpacingBandMin;
    const float BandMax = Enemy->GetEnemyData()->SpacingBandMax;

    // TODO 1: compute the direction FROM the enemy TO the player (normalized).
    
    FVector Direction = (PlayerLoc - EnemyLoc).GetSafeNormal();
    
    // TODO 2: the three-zone decision.
    if (DistToPlayer > BandMax)
    {
        Enemy->AddMovementInput(Direction,1.0f);
    }
    else if (DistToPlayer < BandMin)
    {
        Enemy->AddMovementInput(-Direction,1.0f);
    }
    else
    {
        
    }

    // TODO 3: nothing to finish — spacing runs continuously.
    // This task stays InProgress; the BT's attack decorator will abort it when in range.
    // So: no FinishLatentTask here on the normal path.
}