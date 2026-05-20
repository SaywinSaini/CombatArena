// Fill out your copyright notice in the Description page of Project Settings.


#include "CABTTask_EnemyAttack.h"

#include "AIController.h"
#include "CAEnemyBase.h"

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
	AnimInstance->Montage_Play(Montage);
	
	
	return EBTNodeResult::Succeeded;
}

