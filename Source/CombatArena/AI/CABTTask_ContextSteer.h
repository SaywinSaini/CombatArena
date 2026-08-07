#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "CABTTask_ContextSteer.generated.h"

/**
 * Drives movement from the owner's steering component. The component decides
 * the direction; this task supplies the tactical range band and applies the result.
 */
UCLASS()
class COMBATARENA_API UCABTTask_ContextSteer : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UCABTTask_ContextSteer();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Steering")
	float MovementScale = 1.0f;
};