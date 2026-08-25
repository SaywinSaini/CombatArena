#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "CABTTask_StaggerPunish.generated.h"

/**
 * Combo played against a staggered player. Holds the branch until the montage
 * finishes so the enemy commits to the full punish.
 */
UCLASS()
class COMBATARENA_API UCABTTask_StaggerPunish : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UCABTTask_StaggerPunish();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Punish")
	float MaxRange = 200.0f;
	
	float PunishEndTime = 0.f;
};