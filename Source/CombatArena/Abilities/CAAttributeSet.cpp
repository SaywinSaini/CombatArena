// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/CAAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "AI/CAEnemyBase.h"
#include "Characters/CAPlayerCharacter.h"

void UCAAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0, GetMaxHealth()));

		AActor* Avatar = Data.Target.GetAvatarActor();

		if (ACAEnemyBase* Enemy = Cast<ACAEnemyBase>(Avatar))
		{
			Enemy->CheckPhaseTransition();
		}

		// Trigger death when health reaches zero.
		if (GetHealth() <= 0)
		{
			if (ACAEnemyBase* Enemy = Cast<ACAEnemyBase>(Avatar))
			{
				Enemy->Die();
			}
			else if (ACAPlayerCharacter* Player = Cast<ACAPlayerCharacter>(Avatar))
			{
				Player->Die();
			}
		}
	}
}
