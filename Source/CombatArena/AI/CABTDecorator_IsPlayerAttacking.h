#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "CABTDecorator_IsPlayerAttacking.generated.h"

UCLASS()
class COMBATARENA_API UCABTDecorator_IsPlayerAttacking : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	UCABTDecorator_IsPlayerAttacking();
	
	protected:
	
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
