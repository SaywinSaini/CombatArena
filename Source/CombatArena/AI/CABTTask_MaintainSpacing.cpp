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
    StrafeDirection = FMath::RandBool() ? 1.0f : -1.0f;
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
        BandMin = Enemy->GetEnemyData()->AttackBandMin;
        BandMax = Enemy->GetEnemyData()->AttackBandMax;
    }
    else
    {
        BandMin = Enemy->GetEnemyData()->SpacingBandMin;
        BandMax = Enemy->GetEnemyData()->SpacingBandMax;
    }
    
    const UCAEnemyData* Data = Enemy->GetEnemyData();

    // Run when far outside the band, walk when settling into it.
    const bool bFarFromBand = DistToPlayer > BandMax + Data->SpacingApproachRange;
    
    Enemy->GetCharacterMovement()->MaxWalkSpeed = bFarFromBand ? Data->ChaseSpeed : Data->SpacingSpeed;

    const FVector Direction = (PlayerLoc - EnemyLoc).GetSafeNormal();

    if (DistToPlayer > BandMax)
    {
        Enemy->AddMovementInput(Direction, 1.0f);
    }
    else if (DistToPlayer < BandMin)
    {
        Enemy->AddMovementInput(-Direction, 1.0f);
    }
    else
    {
        // Inside the band: circle the player rather than standing idle.
        // Waiting enemies that hold still read as broken AI.
        const FVector Right = FVector::CrossProduct(FVector::UpVector, Direction).GetSafeNormal();
        Enemy->AddMovementInput(Right * StrafeDirection, Data->StrafeInputScale);
    }
}


EBTNodeResult::Type UCABTTask_MaintainSpacing::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    return Super::AbortTask(OwnerComp, NodeMemory);
}
