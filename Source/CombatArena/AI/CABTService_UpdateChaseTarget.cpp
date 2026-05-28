// Fill out your copyright notice in the Description page of Project Settings.


#include "CABTService_UpdateChaseTarget.h"

#include "BehaviorTree/BlackboardComponent.h"


UCABTService_UpdateChaseTarget::UCABTService_UpdateChaseTarget()
{
	NodeName = "Update Chase Target";
	Interval = 0.1f;
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
	//Current Player velocity
	FVector Velocity = Player->GetVelocity();
	Velocity.Z = 0.f;
	
	//Prevent insane prediction
	Velocity = Velocity.GetClampedToMaxSize(600.0f);
	
	FVector PredictedLocation = Player->GetActorLocation() + Velocity * 0.6f;
	
	UE_LOG(LogTemp, Warning,TEXT("CABTService: Predicted ChaseTarget %s"),*PredictedLocation.ToString());
	
	// Write updated player location to chase target key every interval
	OwnerComp.GetBlackboardComponent()->SetValueAsVector(ChaseTargetKey.SelectedKeyName,PredictedLocation);
}
