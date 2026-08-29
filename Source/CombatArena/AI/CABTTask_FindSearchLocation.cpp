// Fill out your copyright notice in the Description page of Project Settings.


#include "CABTTask_FindSearchLocation.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UCABTTask_FindSearchLocation::UCABTTask_FindSearchLocation()
{
	NodeName = TEXT("Find Search Location");
}

EBTNodeResult::Type UCABTTask_FindSearchLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	APawn* OwnerPawn = OwnerComp.GetAIOwner()->GetPawn();
	
	if (!OwnerPawn) return EBTNodeResult::Failed;
	
	//Read last Known player position from Blackboard
	FVector LastKnownLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector(TEXT("LastKnownLocation"));
	
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	
	if (!NavSystem) return EBTNodeResult::Failed;
	
	FNavLocation SearchLocation;
	
	//Find reachable navigation point around last known position
	NavSystem->GetRandomReachablePointInRadius(LastKnownLocation,SearchRadius,SearchLocation);
	
	//Store generated search point in Blackboard
	OwnerComp.GetBlackboardComponent()->SetValueAsVector(SearchLocationKey.SelectedKeyName,SearchLocation.Location);
	
	return EBTNodeResult::Succeeded;
}

