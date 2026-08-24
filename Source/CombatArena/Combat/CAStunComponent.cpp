#include "CAStunComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/CAAttributeSet.h"
#include "AI/CAEnemyBase.h"
#include "Characters/CAPlayerCharacter.h"

UCAStunComponent::UCAStunComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCAStunComponent::AddStun(float Amount)
{
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	if (!ASC) return;

	const float Max = ASC->GetNumericAttribute(UCAAttributeSet::GetMaxStunAttribute());
	const float Current = ASC->GetNumericAttribute(UCAAttributeSet::GetStunAttribute());

	const float NewStun = FMath::Clamp(Current + Amount, 0.f, Max);
	
	UE_LOG(LogTemp, Warning, TEXT("Stun on %s: +%.1f = %.1f / %.1f"), *GetNameSafe(GetOwner()), Amount, NewStun, Max);
	
	ASC->SetNumericAttributeBase(UCAAttributeSet::GetStunAttribute(), NewStun);

	LastStunTime = GetWorld()->GetTimeSeconds();
	
	if (NewStun >= Max && Max > 0.f)
	{
		if (ACAEnemyBase* Enemy = Cast<ACAEnemyBase>(GetOwner()))
		{
			Enemy->EnterStagger();
		}
		else if (ACAPlayerCharacter* Player = Cast<ACAPlayerCharacter>(GetOwner()))
		{
			Player->EnterStagger();
		}
	}
}

void UCAStunComponent::ResetStun()
{
	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
	{
		ASC->SetNumericAttributeBase(UCAAttributeSet::GetStunAttribute(), 0.f);
	}
}

void UCAStunComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	if (!ASC) return;

	const float Current = ASC->GetNumericAttribute(UCAAttributeSet::GetStunAttribute());
	if (Current <= 0.f) return;

	const float Max = ASC->GetNumericAttribute(UCAAttributeSet::GetMaxStunAttribute());
	if (Max <= 0.f) return;

	const float Fraction = Current / Max;

	float DecayRate = Tier3DecayRate;

	if (Fraction <= Tier1Threshold)
	{
		if (GetWorld()->GetTimeSeconds() - LastStunTime < DecayDelay) return;

		DecayRate = Tier1DecayRate;
	}
	else if (Fraction <= Tier2Threshold)
	{
		DecayRate = Tier2DecayRate;
	}

	ASC->SetNumericAttributeBase(UCAAttributeSet::GetStunAttribute(),
		FMath::Max(Current - DecayRate * DeltaTime, 0.f));
}