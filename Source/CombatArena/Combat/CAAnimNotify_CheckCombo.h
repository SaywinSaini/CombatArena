// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "CAAnimNotify_CheckCombo.generated.h"

/**
 * 
 */
UCLASS()
class COMBATARENA_API UCAAnimNotify_CheckCombo : public UAnimNotify
{
	GENERATED_BODY()
	
	public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,const FAnimNotifyEventReference& EventReference) override;
};
