#include "CABTTask_EvadeDash.h"

#include "AIController.h"
#include "CAEnemyBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/CAEnemyData.h"
#include "Slate/SGameLayerManager.h"

UCABTTask_EvadeDash::UCABTTask_EvadeDash()
{
	NodeName = TEXT("Evade Dash");
	bNotifyTick = false;
}

EBTNodeResult::Type UCABTTask_EvadeDash::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC) return EBTNodeResult::Failed;
	
	ACAEnemyBase* Enemy = Cast<ACAEnemyBase>(AIC->GetPawn());
	if (!Enemy || !Enemy->GetEnemyData()) return EBTNodeResult::Failed;
	
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return EBTNodeResult::Failed;
	
	AActor* Player = Cast<AActor>(BB->GetValueAsObject(TEXT("PlayerActor")));
	if (!Player) return EBTNodeResult::Failed;
	
	const float Now = Enemy->GetWorld()->GetTimeSeconds();
	if (Now - Enemy->GetLastDashTime() < Enemy->GetEnemyData()->EvadeCooldown)
	{
		return EBTNodeResult::Failed;
	}
	
	FVector ToEnemy = (Enemy->GetActorLocation() - Player->GetActorLocation()).GetSafeNormal();
	ToEnemy.Z = 0.f;
	
	FVector SideDir = FVector::CrossProduct(ToEnemy, FVector::UpVector).GetSafeNormal();
	
	if (FMath::RandBool())
	{
		SideDir = -SideDir;
	}
	
	Enemy->LaunchCharacter(SideDir * DashImpulse, true, false);
	Enemy->SetLastDashTime(Now);
	
	return EBTNodeResult::Succeeded;
}
