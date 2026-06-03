// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/CAAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "AI/CAEnemyBase.h"

void UCAAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0, GetMaxHealth()));
		
		// Trigger enemy death when health reaches zero.
		if (GetHealth() <= 0)
		{
			ACAEnemyBase* Enemy = Cast<ACAEnemyBase>(Data.Target.GetAvatarActor());
			if (Enemy)
			{
				Enemy->Die();
			}
		}
	}
	
	if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		//clamp Stamina between 0 and MaxStamina
		SetStamina(FMath::Clamp(GetStamina(), 0, GetMaxStamina()));
	}
	
	
}
