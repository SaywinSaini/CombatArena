// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/CAAttributeSet.h"


void UCAAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	//Check if Health was modified
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		//clamp Health between 0 and MaxHealth
		SetHealth(FMath::Clamp(GetHealth(),0,GetMaxHealth()));
	}
	
	//Check if Stamina was modified
	if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		//clamp Stamina between 0 and MaxStamina
		SetStamina(FMath::Clamp(GetStamina(),0,GetMaxStamina()));
	}
}
