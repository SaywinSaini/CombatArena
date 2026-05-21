// Fill out your copyright notice in the Description page of Project Settings.


#include "CABTService_UpdateChaseTarget.h"

#include "BehaviorTree/BlackboardComponent.h"


UCABTService_UpdateChaseTarget::UCABTService_UpdateChaseTarget()
{
	NodeName = "Update Chase Target";
	Interval = 2.0f;
	RandomDeviation = 0.5f;
}

void UCABTService_UpdateChaseTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	if (!OwnerComp.GetBlackboardComponent()) return;

	// Get player actor from Blackboard
	AActor* Player = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TEXT("PlayerActor")));
		
	if (!Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("CABTService: PlayerActor is null"));
		return;
	}
	
	FVector PlayerLocation = Player->GetActorLocation();
	UE_LOG(LogTemp, Warning, TEXT("CABTService: Writing location %s to ChaseTarget"),
		*PlayerLocation.ToString());

	
	// Write updated player location to chase target key every interval
	OwnerComp.GetBlackboardComponent()->SetValueAsVector(ChaseTargetKey.SelectedKeyName,Player->GetActorLocation());
}
