#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "CABTTask_Slip.generated.h"

/**
 * Upper-body slip away from an incoming attack. Unlike Evade, this reacts to a
 * single strike rather than escaping a combo, and does not move the enemy.
 */
UCLASS()
class COMBATARENA_API UCABTTask_Slip : public UBTTaskNode
{
	GENERATED_BODY()
	
	public:
	UCABTTask_Slip();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
protected:
	
	UPROPERTY(EditAnywhere, Category = "Slip", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float SlipChance = 0.6f;
	
	UPROPERTY(EditAnywhere, Category = "Slip")
	float MaxSlipDistance = 130.0f;
	
	UPROPERTY(EditAnywhere, Category = "Slip")
	float SlipPlayRate = 0.55f;
};
