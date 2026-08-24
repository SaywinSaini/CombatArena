// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "CAAnimNotifyState_PerformTrace.generated.h"

UCLASS()
class COMBATARENA_API UCAAnimNotifyState_PerformTrace : public UAnimNotifyState
{
	GENERATED_BODY()
	
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
public:
	
	UPROPERTY(EditAnywhere, Category = "Trace")
	FName TraceSocketOverride = NAME_None;
	
	UPROPERTY(EditAnywhere, Category = "Trace")
	FName HitReactSection = NAME_None;
	
	UPROPERTY(EditAnywhere, Category = "Trace")
	FName DeathSectionOverride = NAME_None;
	
	UPROPERTY(EditAnywhere, Category = "Trace")
	float StunAmount = 5.0f;
	
	UPROPERTY(EditAnywhere, Category = "Trace")
    float BlockedStunAmount = 10.0f;
};

