#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CABossHealthBarWidget.generated.h"

class UAbilitySystemComponent;
struct FOnAttributeChangeData;
class UProgressBar;
class ACAEnemyBase;

UCLASS()
class COMBATARENA_API UCABossHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Boss")
	void InitializeForBoss(ACAEnemyBase* Boss);
	
protected:
	virtual void NativeDestruct() override;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthBar;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> StunBar;
	
private:
	
	void OnHealthChanged(const FOnAttributeChangeData& Data);
	void OnStunChanged(const FOnAttributeChangeData& Data);
	
	void RefreshHealthBar();
	void RefreshStunBar();
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> CachedASC;
	
	FDelegateHandle HealthChangedHandle;
	FDelegateHandle StunChangedHandle;
	
};
