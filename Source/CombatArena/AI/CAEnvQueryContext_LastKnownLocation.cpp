// Fill out your copyright notice in the Description page of Project Settings.


#include "CAEnvQueryContext_LastKnownLocation.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"

void UCAEnvQueryContext_LastKnownLocation::ProvideContext(FEnvQueryInstance& QueryInstance,
                                                          FEnvQueryContextData& ContextData) const
{
	Super::ProvideContext(QueryInstance, ContextData);
	
	//Get AI Owner running the EQS query
	AAIController* AIController = Cast<AAIController>(QueryInstance.Owner.Get());
	
	if (!AIController) return;
	
	//Access Blackboard component
	UBlackboardComponent* Blackboard = AIController->GetBlackboardComponent();
	
	if (!Blackboard) return;
	
	//Read stored last known player location
	FVector LastKnownLocation = Blackboard->GetValueAsVector(TEXT("LastKnownLocation"));
	
	//Provide vector location to EQS query
	UEnvQueryItemType_Point::SetContextHelper(ContextData,LastKnownLocation);
}
