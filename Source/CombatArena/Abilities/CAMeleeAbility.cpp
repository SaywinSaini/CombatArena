// Fill out your copyright notice in the Description page of Project Settings.


#include "CAMeleeAbility.h"
#include "Characters/CAPlayerCharacter.h"
#include "Combat/CAHitDetectionComponent.h"
#include "GameFramework/Character.h"

UCAMeleeAbility::UCAMeleeAbility()
{
	// Tag this ability so it can be found and activated by tag queries
	FGameplayTagContainer NewTags;
	NewTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Melee")));
	SetAssetTags(NewTags);
}

void UCAMeleeAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                      const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	ACAPlayerCharacter* Character = Cast<ACAPlayerCharacter>(ActorInfo->AvatarActor.Get());
	
	if (!Character) return;
	
	//Start hit detection
	if (UCAHitDetectionComponent* HitDetection = Character->GetHitDetectionComponent())
	{
		HitDetection->StartTrace();
	}
	
	Character->PlayAnimMontage(AttackMontage);
	
	//Stop hit detection and end ability
	
	if (UCAHitDetectionComponent* HitDetection = Character->GetHitDetectionComponent())
	{
		HitDetection->StopTrace();
	}
	
	EndAbility(Handle,ActorInfo,ActivationInfo,true,false);
	
	
	
}
