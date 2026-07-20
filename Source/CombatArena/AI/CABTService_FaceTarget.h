#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "CABTService_FaceTarget.generated.h"

UCLASS()
class COMBATARENA_API UCABTService_FaceTarget : public UBTService
{
	GENERATED_BODY()

public:
	UCABTService_FaceTarget();

protected:
	
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};