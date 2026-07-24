#include "CABTService_UpdateCombatStatus.h"
#include "AIController.h"
#include "CAEnemyBase.h"
#include "Characters/CAEnemyData.h"
#include "Core/CAGameplayTags.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

UCABTService_UpdateCombatStatus::UCABTService_UpdateCombatStatus()
{
	NodeName = TEXT("Update Combat Status");
	Interval = 0.1f;
	RandomDeviation = 0.0f;
}

void UCABTService_UpdateCombatStatus::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC) return;
	
	ACAEnemyBase* Enemy = Cast<ACAEnemyBase>(AIC->GetPawn());
	if (!Enemy || !Enemy->GetEnemyData()) return;
	
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return;
	
	AActor* Player = Cast<AActor>(BB->GetValueAsObject(TEXT("PlayerActor")));
	if (!Player)
	{
		BB->SetValueAsBool(TEXT("bIsInAttackRange"), false);
        BB->SetValueAsBool(TEXT("bIsPlayerAttacking"), false);
		BB->SetValueAsBool(TEXT("bAttackCooldownReady"),false);
		BB->SetValueAsBool(TEXT("bIsPlayerComboing"), false);
		return;
	}
	
	const float Distance = FVector::Dist(Enemy->GetActorLocation(), Player->GetActorLocation());
	const bool bInRange = Distance <= Enemy->GetEnemyData()->AttackRange;

	// Attack is only allowed if in range AND the cooldown has elapsed since the last attack.
	const float TimeSinceAttack = Enemy->GetWorld()->GetTimeSeconds() - Enemy->GetLastAttackTime();
	const bool bCooldownReady = TimeSinceAttack >= Enemy->GetEnemyData()->AttackCooldown;

	BB->SetValueAsBool(TEXT("bIsInAttackRange"), bInRange && bCooldownReady);
	BB->SetValueAsBool(TEXT("bAttackCooldownReady"), bCooldownReady);
	
	bool bPlayerAttacking = false;
	bool bPlayerComboing = false;
	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Player))
	{
		if (UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
		{
			bPlayerAttacking = ASC->HasMatchingGameplayTag(CATags::State_Attacking);
			bPlayerComboing = ASC->HasMatchingGameplayTag(CATags::State_Comboing);
		}
	}
	BB->SetValueAsBool(TEXT("bIsPlayerAttacking"), bPlayerAttacking);
	BB->SetValueAsBool(TEXT("bIsPlayerComboing"), bPlayerComboing);

}
