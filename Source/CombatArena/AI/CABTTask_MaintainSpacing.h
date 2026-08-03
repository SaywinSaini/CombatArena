#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "CABTTask_MaintainSpacing.generated.h"

UCLASS()
class COMBATARENA_API UCABTTask_MaintainSpacing : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UCABTTask_MaintainSpacing();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
	virtual EBTNodeResult::Type AbortTask(
	UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory) override;
	
	// Per-instance circling direction so enemies don't all orbit the same way.
	float StrafeDirection = 1.0f;
};