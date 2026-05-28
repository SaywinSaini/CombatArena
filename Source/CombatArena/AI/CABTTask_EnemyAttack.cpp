// Fill out your copyright notice in the Description page of Project Settings.


#include "CABTTask_EnemyAttack.h"

#include "AIController.h"
#include "CAEnemyBase.h"
#include "BehaviorTree/BlackboardComponent.h"

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
	UE_LOG(LogTemp, Warning, TEXT("CABTTask_EnemyAttack: Playing montage %s"), 
	   *Montage->GetName());
	
	AActor* Player = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TEXT("PlayerActor")));
	if (!Player)
	{
		return EBTNodeResult::Failed;
	}
	
	// Check attack range before committing to montage
	
	const float Distance = FVector::Dist(Player->GetActorLocation(), Enemy->GetActorLocation());
	
	// Abort attack if target moved outside melee range
	if (Distance > 600.0f)
	{
		UE_LOG(LogTemp, Warning,TEXT("EnemyAttack: Player out of range"));

		return EBTNodeResult::Failed;
	}
	// Stop navigation movement before playing attack montage
	OwnerComp.GetAIOwner()->StopMovement();
	
	// Rotate enemy toward target before attack execution
	FVector Direction = (Player->GetActorLocation()-Enemy->GetActorLocation()).GetSafeNormal();
	Direction.Z =0.0f;
	
	FRotator FaceRotation = Direction.Rotation();
	Enemy->SetActorRotation(FaceRotation);
	
	// Prevent BT branch switching during attack montage
	OwnerComp.PauseLogic("Attacking");
	AnimInstance->Montage_Play(Montage);
	
	FOnMontageEnded MontageEndedDelegate;
	MontageEndedDelegate.BindLambda([this, &OwnerComp](UAnimMontage* Montage, bool bInterrupted)
	{   OwnerComp.ResumeLogic("Attacking");
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	});
	AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, Montage);

	
	return EBTNodeResult::InProgress;
}

