// Fill out your copyright notice in the Description page of Project Settings.


#include "CABTTask_Evade.h"

#include "AIController.h"
#include "CAEnemyBase.h"
#include "BehaviorTree/BlackboardComponent.h"

UCABTTask_Evade::UCABTTask_Evade()
{
	NodeName = TEXT("Evade");
	bNotifyTick = true;
	
}

EBTNodeResult::Type UCABTTask_Evade::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::InProgress;
}

void UCABTTask_Evade::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC) { FinishLatentTask(OwnerComp, EBTNodeResult::Failed); return; }
	
	ACAEnemyBase* Enemy = Cast<ACAEnemyBase>(AIC->GetPawn());
	if (!Enemy) { FinishLatentTask(OwnerComp, EBTNodeResult::Failed); return; }
	
	AActor* Player = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TEXT("PlayerActor")));
	if (!Player) { FinishLatentTask(OwnerComp, EBTNodeResult::Failed); return; }
	
	const FVector PlayerLocation = Player->GetActorLocation();
	const FVector EnemyLocation  = Enemy->GetActorLocation();
	
	const FVector ToPlayer = (PlayerLocation - EnemyLocation).GetSafeNormal();
	
	Enemy->AddMovementInput(-ToPlayer,1.0f);
}
