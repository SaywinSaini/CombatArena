// Fill out your copyright notice in the Description page of Project Settings.


#include "CAAnimNotifyState_PerformTrace.h"

#include "CAHitDetectionComponent.h"
#include "Characters/CAPlayerCharacter.h"

void UCAAnimNotifyState_PerformTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                  float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	if (!MeshComp || !MeshComp->GetOwner()) return;
	
	if (UCAHitDetectionComponent* HitDetectionComp = MeshComp->GetOwner()->FindComponentByClass<UCAHitDetectionComponent>())
	{
		HitDetectionComp->SetTraceSocket(TraceSocketOverride);
		HitDetectionComp->SetHitReactSection(HitReactSection);
		HitDetectionComp->SetDeathSection(DeathSectionOverride);
		HitDetectionComp->StartTrace();
	}
}

void UCAAnimNotifyState_PerformTrace::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (!MeshComp || !MeshComp->GetOwner()) return;
	
	if (UCAHitDetectionComponent* HitDetectionComp = MeshComp->GetOwner()->FindComponentByClass<UCAHitDetectionComponent>())
	{
		HitDetectionComp->PerformTrace();
	}
}

void UCAAnimNotifyState_PerformTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp || !MeshComp->GetOwner()) return;
	
	if (UCAHitDetectionComponent* HitDetectionComp = MeshComp->GetOwner()->FindComponentByClass<UCAHitDetectionComponent>())
	{
		HitDetectionComp->StopTrace();
		HitDetectionComp->ResetTraceSocket();
	}
}
