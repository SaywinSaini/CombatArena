#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "CABTTask_EvadeDash.generated.h"


UCLASS()
class COMBATARENA_API UCABTTask_EvadeDash : public UBTTaskNode
{
	GENERATED_BODY()
	
	public:
	UCABTTask_EvadeDash();
	
	protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	UPROPERTY(EditAnywhere,Category = "Evade")
	float DashImpulse = 700.0f;
};
