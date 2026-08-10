#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "Characters/CAEnemyData.h"
#include "CABTDecorator_IsEnemyType.generated.h"

/**
 * Passes when the owning enemy matches the configured type. Used to gate
 * branches that only apply to specific enemy types, such as evasion.
 */
UCLASS()
class COMBATARENA_API UCABTDecorator_IsEnemyType : public UBTDecorator
{
	GENERATED_BODY()
	
	public:
	UCABTDecorator_IsEnemyType();
	
	protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	
	UPROPERTY(EditAnywhere,Category = "Condition")
	EEnemyType RequiredType = EEnemyType::Aggressive;
};
