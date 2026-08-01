// Fill out your copyright notice in the Description page of Project Settings.


#include "CABTTask_FindPatrolLocation.h"

#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "CAEnemyBase.h"
#include "Characters/CAEnemyData.h"
#include "GameFramework/CharacterMovementComponent.h"


UCABTTask_FindPatrolLocation::UCABTTask_FindPatrolLocation()
{
	NodeName = TEXT("Find Patrol Location");
}

EBTNodeResult::Type UCABTTask_FindPatrolLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC) return EBTNodeResult::Failed;
	
	APawn* OwnerPawn = AIC->GetPawn();
	if (!OwnerPawn) return EBTNodeResult::Failed;
	
	// Each task sets its own speed rather than assuming a previous task restored it.
	if (ACAEnemyBase* Enemy = Cast<ACAEnemyBase>(OwnerPawn))
	{
		if (Enemy->GetEnemyData())
		{
			Enemy->GetCharacterMovement()->MaxWalkSpeed = Enemy->GetEnemyData()->PatrolSpeed;
		}
	}
	
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSystem) return EBTNodeResult::Failed;
	
	FNavLocation RandomLocation;
	NavSystem->GetRandomReachablePointInRadius(OwnerPawn->GetActorLocation(), PatrolRadius, RandomLocation);
	
	OwnerComp.GetBlackboardComponent()->SetValueAsVector(PatrolLocationKey.SelectedKeyName,RandomLocation.Location);
	
	return EBTNodeResult::Succeeded;
}
