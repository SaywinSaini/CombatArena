// Fill out your copyright notice in the Description page of Project Settings.


#include "CABTTask_EnemyAttack.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AIController.h"
#include "CAEnemyBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Core/CAGameMode.h"


UCABTTask_EnemyAttack::UCABTTask_EnemyAttack()
{
	NodeName = TEXT("Enemy Attack");
}

EBTNodeResult::Type UCABTTask_EnemyAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Get the enemy pawn that owns this BT
	ACAEnemyBase* Enemy = Cast<ACAEnemyBase>(OwnerComp.GetAIOwner()->GetPawn());
	if (!Enemy)
	{
		UE_LOG(LogTemp, Error, TEXT("CABTTask_EnemyAttack: Enemy cast failed"));
		return EBTNodeResult::Failed;
	}
	
	if (Enemy->IsReacting() || Enemy->IsStaggered())
	{
		return EBTNodeResult::Failed;
	}
	
	UAnimInstance* AnimInstance = Enemy->GetMesh()->GetAnimInstance();
	if (!AnimInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("CABTTask_EnemyAttack: AnimInstance is null"));
		return EBTNodeResult::Failed;
	}
	
	UAnimMontage* Montage = Enemy->GetAttackMontage();
	if (!Montage)
	{
		UE_LOG(LogTemp, Error, TEXT("CABTTask_EnemyAttack: AttackMontage is null"));
		return EBTNodeResult::Failed;
	}
	
	AActor* Player = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TEXT("PlayerActor")));
	if (!Player)
	{
		return EBTNodeResult::Failed;
	}
	
	ACAGameMode* GameMode = Cast<ACAGameMode>(Enemy->GetWorld()->GetAuthGameMode());
	if (!GameMode || !GameMode->TryClaimAttackToken(Enemy))
	{
		return EBTNodeResult::Failed;
	}
	
	// Stop navigation movement before playing attack montage
	OwnerComp.GetAIOwner()->StopMovement();
	
	// Rotate enemy toward target before attack execution
	FVector Direction = (Player->GetActorLocation()-Enemy->GetActorLocation()).GetSafeNormal();
	Direction.Z =0.0f;
	Enemy->SetActorRotation(Direction.Rotation());
	
	Enemy->SetLastAttackTime(Enemy->GetWorld()->GetTimeSeconds());
	
	// Prevent BT branch switching during attack montage
	OwnerComp.PauseLogic("Attacking");
	AnimInstance->Montage_Play(Montage);
	
	TWeakObjectPtr<UBehaviorTreeComponent> WeakComp = &OwnerComp;
	TWeakObjectPtr<ACAEnemyBase> WeakEnemy = Enemy;
	TWeakObjectPtr<ACAGameMode> WeakGameMode = GameMode;

	FOnMontageEnded MontageEndedDelegate;
	MontageEndedDelegate.BindLambda(
		[this, WeakComp, WeakEnemy, WeakGameMode](UAnimMontage*, bool bInterrupted)
	{
		if (!WeakComp.IsValid()) return;

		if (WeakGameMode.IsValid() && WeakEnemy.IsValid())
		{
			WeakGameMode->ReleaseAttackToken(WeakEnemy.Get());
		}

		WeakComp->ResumeLogic("Attacking");
		FinishLatentTask(*WeakComp, bInterrupted ? EBTNodeResult::Aborted : EBTNodeResult::Succeeded);
	});
	AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, Montage);
	
	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UCABTTask_EnemyAttack::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AAIController* AIC = OwnerComp.GetAIOwner())
	{
		if (ACAEnemyBase* Enemy = Cast<ACAEnemyBase>(AIC->GetPawn()))
		{
			if (ACAGameMode* GameMode = Cast<ACAGameMode>(Enemy->GetWorld()->GetAuthGameMode()))
			{
				GameMode->ReleaseAttackToken(Enemy);
			}
		}
	}

	OwnerComp.ResumeLogic("Attacking");
	return Super::AbortTask(OwnerComp, NodeMemory);
}

