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
			
			bIsBlocking = ASC->HasMatchingGameplayTag(CATags::State_Blocking);
		}
	}
}

void UCAAnimInstance::OnBlockTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	bIsBlocking = NewCount > 0;
}

