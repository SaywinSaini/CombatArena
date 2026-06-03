// Fill out your copyright notice in the Description page of Project Settings.


#include "CAProjectileAbility.h"

#include "GameFramework/Character.h"

// Assign the gameplay tag used to identify this ability as a ranged attack.
UCAProjectileAbility::UCAProjectileAbility()
{
	FGameplayTagContainer NewTags;
	NewTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Ranged")));
    SetAssetTags(NewTags);
}

void UCAProjectileAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	
	if (!Character) return;
	
	FVector SpawnLocation = Character->GetActorLocation() + (Character->GetActorForwardVector() * SpawnOffset);
	
	FRotator SpawnRotation = Character->GetActorRotation();
	
	if (!ProjectileClass) return;
	
	GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnLocation, SpawnRotation);
	
	EndAbility(Handle,ActorInfo,ActivationInfo,true,false);
}
