// Fill out your copyright notice in the Description page of Project Settings.


#include "CABTDecorator_IsPlayerAttacking.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/CAPlayerCharacter.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Core/CAGameplayTags.h"

UCABTDecorator_IsPlayerAttacking::UCABTDecorator_IsPlayerAttacking()
{
	NodeName = TEXT("Is Player Attacking");
	bNotifyTick = true;
}
bool UCABTDecorator_IsPlayerAttacking::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory) const
{
	return false;
}
