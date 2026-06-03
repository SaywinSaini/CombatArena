// Fill out your copyright notice in the Description page of Project Settings.


#include "CABlockAbility.h"

UCABlockAbility::UCABlockAbility()
{
	// Register gameplay tags associated with this ability.
	FGameplayTagContainer NewTags;
	NewTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Block")));
	SetAssetTags(NewTags);
}

void UCABlockAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UCABlockAbility::OnBlockReleased(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	EndAbility(Handle,ActorInfo,ActivationInfo,true,false);
}
