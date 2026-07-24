// Fill out your copyright notice in the Description page of Project Settings.


#include "CAMeleeAbility.h"
#include "Characters/CAPlayerCharacter.h"
#include "Core/CAGameplayTags.h"
#include "Combat/CAHitDetectionComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Combat/CATargetingComponent.h"


UCAMeleeAbility::UCAMeleeAbility()
{
	// Tag this ability so it can be found and activated by tag queries
	FGameplayTagContainer NewTags;
	NewTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Melee")));
	SetAssetTags(NewTags);
	
	ActivationOwnedTags.AddTag(CATags::State_Attacking);
}

void UCAMeleeAbility::SetComboInputReceived()
{
	bComboInputReceived = true;
}

void UCAMeleeAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	ACAPlayerCharacter* Character = Cast<ACAPlayerCharacter>(ActorInfo->AvatarActor.Get());
	
	if (!Character)
	{
		EndAbility(Handle,ActorInfo,ActivationInfo,true,true);
		return;
	}
	// Melee is a grounded ability — refuse to activate mid-air
	if (Character->GetCharacterMovement() && Character->GetCharacterMovement()->IsFalling())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	if (!CommitAbility(Handle,ActorInfo,ActivationInfo))
	{
		EndAbility(Handle,ActorInfo,ActivationInfo,true,true);
		return;
	}
	
	if (UCAHitDetectionComponent* HitDetection = Character->GetHitDetectionComponent())
	{
		HitDetection->StartTrace();
	}
	
	
	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,NAME_None,AttackMontage,1.0f,ComboSections.IsValidIndex(0) ? ComboSections[0] :NAME_None);
	
	
	MontageTask->OnCompleted.AddDynamic(this, &UCAMeleeAbility::OnMontageCompleted);
	MontageTask->OnCancelled.AddDynamic(this, &UCAMeleeAbility::OnMontageCancelled);
	MontageTask->OnInterrupted.AddDynamic(this, &UCAMeleeAbility::OnMontageCancelled);
	
	// Ability execution continues through montage task delegates after activation.
	MontageTask->ReadyForActivation();
	
}
void UCAMeleeAbility::AdvanceCombo()
{
	// End the combo chain if no follow-up input was received before the notify window.	
if (!bComboInputReceived)
{
	StopAbility();
	return;
}
	bComboInputReceived = false;
	ComboIndex++;
	if (!ComboSections.IsValidIndex(ComboIndex))
	{
		StopAbility();
		return;
	}
	
	 if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	 {
	 	if (!ASC->HasMatchingGameplayTag(CATags::State_Comboing))
	 	{
	 		ASC->AddLooseGameplayTag(CATags::State_Comboing);
	 	}
	 }
	
	ACAPlayerCharacter* Character = Cast<ACAPlayerCharacter>(GetCurrentActorInfo()->AvatarActor.Get());
	if (Character)
	{
		UCAHitDetectionComponent* HitDetection = Character->GetHitDetectionComponent();
		if (HitDetection)
		{
			HitDetection->StartTrace();
		}
		UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{   
			AnimInstance->Montage_JumpToSection(ComboSections[ComboIndex],AttackMontage);
		}
	}
}


void UCAMeleeAbility::OnMontageCompleted()
{
	StopAbility();
}

void UCAMeleeAbility::OnMontageCancelled()
{  
	StopAbility();
}

void UCAMeleeAbility::StopAbility()
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->RemoveLooseGameplayTag(CATags::State_Comboing, 100);
	}
	
	if (const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo())
	{
		ACAPlayerCharacter* Character = Cast<ACAPlayerCharacter>(ActorInfo->AvatarActor.Get());
		if (Character)
		{
			if (UCAHitDetectionComponent* HitDetection = Character->GetHitDetectionComponent())
			{
				HitDetection->StopTrace();
			}
		}
	}
	ComboIndex = 0;
	bComboInputReceived = false;
	const FGameplayAbilitySpecHandle Handle = GetCurrentAbilitySpecHandle();
	const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo();
	const FGameplayAbilityActivationInfo ActivationInfo = GetCurrentActivationInfo();
    
	EndAbility(Handle,ActorInfo,ActivationInfo,true,false);
}