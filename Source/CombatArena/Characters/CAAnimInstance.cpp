#include "CAAnimInstance.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Core/CAGameplayTags.h"

void UCAAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	if (APawn* Owner = TryGetPawnOwner())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner))
		{
			ASC->RegisterGameplayTagEvent(CATags::State_Blocking,EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UCAAnimInstance::OnBlockTagChanged);
          
			ASC->RegisterGameplayTagEvent(CATags::State_Dead,EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UCAAnimInstance::OnDeadTagChanged);
          
			bIsBlocking = ASC->HasMatchingGameplayTag(CATags::State_Blocking);
          
			bIsDead = ASC->HasMatchingGameplayTag(CATags::State_Dead);
		}
	}
}

void UCAAnimInstance::OnBlockTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	bIsBlocking = NewCount > 0;
}

void UCAAnimInstance::OnDeadTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	bIsDead = NewCount > 0;
}

