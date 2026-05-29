// Fill out your copyright notice in the Description page of Project Settings.


#include "CAAnimNotify_CheckCombo.h"
#include "AbilitySystemComponent.h"
#include "Abilities/CAMeleeAbility.h"
#include "GameplayTagContainer.h"
#include "Characters/CAPlayerCharacter.h"

void UCAAnimNotify_CheckCombo::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation,EventReference);
	
	if (!MeshComp) return;

	ACAPlayerCharacter* Player = Cast<ACAPlayerCharacter>(MeshComp->GetOwner());
	if (!Player) return;

	if (UCAMeleeAbility* MeleeAbility = Player->GetActiveMeleeAbility())
	{
		MeleeAbility->AdvanceCombo();
	}
}
