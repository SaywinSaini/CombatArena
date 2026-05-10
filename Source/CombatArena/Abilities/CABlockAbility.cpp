// Fill out your copyright notice in the Description page of Project Settings.


#include "CABlockAbility.h"

UCABlockAbility::UCABlockAbility()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Block")));
}

void UCABlockAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	UE_LOG(LogTemp, Warning, TEXT("Block ability activated"));
}

void UCABlockAbility::OnBlockReleased(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	EndAbility(Handle,ActorInfo,ActivationInfo,true,false);
}
