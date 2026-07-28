#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameplayTagContainer.h"
#include "CAAnimInstance.generated.h"



UCLASS()
class COMBATARENA_API UCAAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	
protected:
	
	UPROPERTY(BlueprintReadOnly, Category = "State")
	bool bIsBlocking = false;
	
private:
	void OnBlockTagChanged(const FGameplayTag Tag, int32 NewCount);
};
