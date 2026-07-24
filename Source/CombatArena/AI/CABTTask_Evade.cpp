// Fill out your copyright notice in the Description page of Project Settings.


#include "CABTTask_Evade.h"
#include "Characters/CAEnemyData.h"
#include "AIController.h"
#include "CAEnemyBase.h"
#include "BehaviorTree/BlackboardComponent.h"

UCABTTask_Evade::UCABTTask_Evade()
{
	NodeName = TEXT("Evade");
	bNotifyTick = false;
	
}

EBTNodeResult::Type UCABTTask_Evade::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC) return EBTNodeResult::Failed;
	
	ACAEnemyBase* Enemy = Cast<ACAEnemyBase>(AIC->GetPawn());
	if (!Enemy) return EBTNodeResult::Failed;
	
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return EBTNodeResult::Failed;
	
	AActor* Player = Cast<AActor>(BB->GetValueAsObject(TEXT("PlayerActor")));
	if (!Player) return EBTNodeResult::Failed;
	
	const float Now = Enemy->GetWorld()->GetTimeSeconds();
	if (Now - Enemy->GetLastEvadeTime() < Enemy->GetEnemyData()->EvadeCooldown)
	{
		return EBTNodeResult::Failed;
	}
	
	FVector Away = (Enemy->GetActorLocation() - Player->GetActorLocation()).GetSafeNormal();
	Away.Z = 0.f;
	
	Enemy->LaunchCharacter(Away * EvadeImpulse, true, false);
	Enemy->SetLastEvadeTime(Now);
	Enemy->ApplyDashInvulnerability(0.3f);

	
	return EBTNodeResult::Succeeded;
}

