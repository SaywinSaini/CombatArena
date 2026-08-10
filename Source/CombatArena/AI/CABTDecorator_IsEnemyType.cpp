#include "CABTDecorator_IsEnemyType.h"

#include "AIController.h"
#include "CAEnemyBase.h"

UCABTDecorator_IsEnemyType::UCABTDecorator_IsEnemyType()
{
	NodeName = TEXT("Is Enemy Type");
}

bool UCABTDecorator_IsEnemyType::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC) return false;
	
	const ACAEnemyBase* Enemy = Cast<ACAEnemyBase>(AIC->GetPawn());
	if (!Enemy || !Enemy->GetEnemyData()) return false;
	
	return Enemy->GetEnemyData()->EnemyType == RequiredType;
}
