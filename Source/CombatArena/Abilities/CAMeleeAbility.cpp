// Fill out your copyright notice in the Description page of Project Settings.


#include "CAMeleeAbility.h"

#include "GameFramework/Character.h"

void UCAMeleeAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                      const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	
	if (!Character) return;
	
	Character->PlayAnimMontage(AttackMontage);
	
	EndAbility(Handle,ActorInfo,ActivationInfo,true,false);
	
}
