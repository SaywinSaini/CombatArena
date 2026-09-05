#include "CABossHealthBarWidget.h"

#include "AbilitySystemComponent.h"
#include "Abilities/CAAttributeSet.h"
#include "AI/CAEnemyBase.h"
#include "Components/ProgressBar.h"


void UCABossHealthBarWidget::InitializeForBoss(ACAEnemyBase* Boss)
{
	if (!IsValid(Boss))
	{
		return;
	}
	
	CachedASC = Boss->GetAbilitySystemComponent();
	if (!CachedASC)
	{
		return;
	}
	
	HealthChangedHandle = CachedASC->GetGameplayAttributeValueChangeDelegate(UCAAttributeSet::GetHealthAttribute()).AddUObject(this, &UCABossHealthBarWidget::OnHealthChanged);
	
	StunChangedHandle = CachedASC->GetGameplayAttributeValueChangeDelegate(
		UCAAttributeSet::GetStunAttribute()).AddUObject(
			this, &UCABossHealthBarWidget::OnStunChanged);
	
	RefreshHealthBar();
	RefreshStunBar();
}

void UCABossHealthBarWidget::NativeDestruct()
{
	if (CachedASC)
	{
		CachedASC->GetGameplayAttributeValueChangeDelegate(
			UCAAttributeSet::GetHealthAttribute()).Remove(HealthChangedHandle);

		CachedASC->GetGameplayAttributeValueChangeDelegate(
			UCAAttributeSet::GetStunAttribute()).Remove(StunChangedHandle);
	}

	CachedASC = nullptr;
	
	Super::NativeDestruct();
}

void UCABossHealthBarWidget::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	RefreshHealthBar();
}

void UCABossHealthBarWidget::OnStunChanged(const FOnAttributeChangeData& Data)
{
	RefreshStunBar();
}

void UCABossHealthBarWidget::RefreshHealthBar()
{
	UE_LOG(LogTemp, Warning, TEXT("RefreshHealth: ASC=%d Bar=%d"),
		CachedASC != nullptr, HealthBar != nullptr);

	if (!CachedASC || !HealthBar) return;

	const float Max = CachedASC->GetNumericAttribute(UCAAttributeSet::GetMaxHealthAttribute());
	const float Current = CachedASC->GetNumericAttribute(UCAAttributeSet::GetHealthAttribute());

	if (Max <= 0.f) return;

	HealthBar->SetPercent(FMath::Clamp(Current / Max, 0.f, 1.f));
}

void UCABossHealthBarWidget::RefreshStunBar()
{
	if (!CachedASC || !StunBar)
	{
		return;
	}

	const float Max = CachedASC->GetNumericAttribute(UCAAttributeSet::GetMaxStunAttribute());
	if (Max <= 0.f)
	{
		return;
	}

	const float Current = CachedASC->GetNumericAttribute(UCAAttributeSet::GetStunAttribute());
	StunBar->SetPercent(FMath::Clamp(Current / Max, 0.f, 1.f));
}
