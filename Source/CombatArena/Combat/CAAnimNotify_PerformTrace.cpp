// Fill out your copyright notice in the Description page of Project Settings.


#include "CAAnimNotify_PerformTrace.h"

#include "CAHitDetectionComponent.h"
#include "Characters/CAPlayerCharacter.h"


void UCAAnimNotify_PerformTrace::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (!MeshComp) return;
	
	ACAPlayerCharacter* Player = Cast<ACAPlayerCharacter>(MeshComp->GetOwner());
	
	if (!Player) return;
	
	if (UCAHitDetectionComponent* HitDetection = Player->GetHitDetectionComponent())
	{
	  HitDetection->PerformTrace();	
	}
}
