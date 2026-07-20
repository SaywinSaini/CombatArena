
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "CABTDecorator_IsInAttackRange.generated.h"


UCLASS()
class COMBATARENA_API UCABTDecorator_IsInAttackRange : public UBTDecorator
{
	GENERATED_BODY()
	
	public:
	UCABTDecorator_IsInAttackRange();
	
	protected:
	
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	

private:
	
	// Maximum distance from player to trigger attack
	UPROPERTY(EditAnywhere,Category = "AI")
	float AttackRange = 200.0f;
	
};
