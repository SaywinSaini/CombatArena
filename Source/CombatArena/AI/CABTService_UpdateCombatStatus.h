#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "CABTService_UpdateCombatStatus.generated.h"

/**
 * 
 */
UCLASS()
class COMBATARENA_API UCABTService_UpdateCombatStatus : public UBTService
{
	GENERATED_BODY()
	
	public:
	UCABTService_UpdateCombatStatus();
	
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
