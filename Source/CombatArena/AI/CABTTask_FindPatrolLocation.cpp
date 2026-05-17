// Fill out your copyright notice in the Description page of Project Settings.


#include "CABTTask_FindPatrolLocation.h"

#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"


UCABTTask_FindPatrolLocation::UCABTTask_FindPatrolLocation()
{
	NodeName = TEXT("Find Patrol Location");
}

EBTNodeResult::Type UCABTTask_FindPatrolLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	APawn* OwnerPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!OwnerPawn) return EBTNodeResult::Failed;
	
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSystem) return EBTNodeResult::Failed;
	
	FNavLocation RandomLocation;
	NavSystem->GetRandomReachablePointInRadius(OwnerPawn->GetActorLocation(), PatrolRadius, RandomLocation);
	
	OwnerComp.GetBlackboardComponent()->SetValueAsVector(PatrolLocationKey.SelectedKeyName,RandomLocation.Location);
	
	return EBTNodeResult::Succeeded;
}
