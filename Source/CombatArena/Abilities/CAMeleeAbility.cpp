// Fill out your copyright notice in the Description page of Project Settings.


#include "CAMeleeAbility.h"
#include "Characters/CAPlayerCharacter.h"
#include "Combat/CAHitDetectionComponent.h"
#include "Combat/CATargetingComponent.h"


UCAMeleeAbility::UCAMeleeAbility()
{
	// Tag this ability so it can be found and activated by tag queries
	FGameplayTagContainer NewTags;
	NewTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Melee")));
	SetAssetTags(NewTags);
}

void UCAMeleeAbility::SetComboInputReceived()
{
	UE_LOG(LogTemp, Warning, TEXT("SetComboInputReceived called"));
	bComboInputReceived = true;
}

void UCAMeleeAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                      const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	//Get the character
	ACAPlayerCharacter* Character = Cast<ACAPlayerCharacter>(ActorInfo->AvatarActor.Get());
	
	if (!Character)
	{
		EndAbility(Handle,ActorInfo,ActivationInfo,true,true);
		return;
	}
	
	//CommitAbility checks cooldown and costs - must be called before doing any work
	
	if (!CommitAbility(Handle,ActorInfo,ActivationInfo))
	{
		EndAbility(Handle,ActorInfo,ActivationInfo,true,true);
		return;
	}
	
	//Tell hit detection we are starting a swing
	if (UCAHitDetectionComponent* HitDetection = Character->GetHitDetectionComponent())
	{
		HitDetection->StartTrace();
	}
	
	//Create the async montage task
	
	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,NAME_None,AttackMontage,1.0f,ComboSections.IsValidIndex(0) ? ComboSections[0] :NAME_None);
	
	//Bind delegates — these fire when montage ends
	
	MontageTask->OnCompleted.AddDynamic(this, &UCAMeleeAbility::OnMontageCompleted);
	MontageTask->OnCancelled.AddDynamic(this, &UCAMeleeAbility::OnMontageCancelled);
	MontageTask->OnInterrupted.AddDynamic(this, &UCAMeleeAbility::OnMontageCancelled);
	
	//ability suspends here, montage begins playing
	MontageTask->ReadyForActivation();
	
}
void UCAMeleeAbility::AdvanceCombo()
{
	UE_LOG(LogTemp, Warning,
	TEXT("AdvanceCombo called. InputReceived=%s"),
	bComboInputReceived ? TEXT("TRUE") : TEXT("FALSE"));
	
	UE_LOG(LogTemp, Warning, TEXT("AdvanceCombo called"));
	
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
	//Play next section
	ACAPlayerCharacter* Character = Cast<ACAPlayerCharacter>(GetCurrentActorInfo()->AvatarActor.Get());
	if (Character)
	{
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