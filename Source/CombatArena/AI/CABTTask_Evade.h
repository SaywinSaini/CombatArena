#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "CABTTask_Evade.generated.h"

UCLASS()
class COMBATARENA_API UCABTTask_Evade : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	
	UCABTTask_Evade();
	
	protected:
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
