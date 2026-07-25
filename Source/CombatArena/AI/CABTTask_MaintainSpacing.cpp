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
    
    if (Enemy->IsReacting()) return;
    
    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!BB) { FinishLatentTask(OwnerComp, EBTNodeResult::Failed); return; }

    // Get the player from the Blackboard
    AActor* Player = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TEXT("PlayerActor")));
    if (!Player) { FinishLatentTask(OwnerComp, EBTNodeResult::Failed); return; }

    const FVector EnemyLoc  = Enemy->GetActorLocation();
    const FVector PlayerLoc = Player->GetActorLocation();
    const float   DistToPlayer = FVector::Dist(EnemyLoc, PlayerLoc);

    const bool bCooldownReady = BB->GetValueAsBool(TEXT("bAttackCooldownReady"));
    
    float BandMin, BandMax;
    if (bCooldownReady)
    {
        BandMin = 100.f;
        BandMax = 130.f;
    }
    else
    {
        BandMin = Enemy->GetEnemyData()->SpacingBandMin;
        BandMax = Enemy->GetEnemyData()->SpacingBandMax;
    }

    const FVector Direction = (PlayerLoc - EnemyLoc).GetSafeNormal();

    if (DistToPlayer > BandMax)
    {
        Enemy->AddMovementInput(Direction, 1.0f);
    }
    else if (DistToPlayer < BandMin)
    {
        Enemy->AddMovementInput(-Direction, 1.0f);
    }
    // else: inside the band, hold position
}


EBTNodeResult::Type UCABTTask_MaintainSpacing::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    return Super::AbortTask(OwnerComp, NodeMemory);
}
