#include "CABTTask_StaggerPunish.h"

#include "AIController.h"
#include "CAEnemyBase.h"
#include "Characters/CAEnemyData.h"
#include "Characters/CAPlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UCABTTask_StaggerPunish::UCABTTask_StaggerPunish()
{
	NodeName = TEXT("Stagger Punish");
	bNotifyTick = true;
}

EBTNodeResult::Type UCABTTask_StaggerPunish::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UE_LOG(LogTemp, Warning, TEXT("StaggerPunish: executing"));
	
	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC) return EBTNodeResult::Failed;

	ACAEnemyBase* Enemy = Cast<ACAEnemyBase>(AIC->GetPawn());
	if (!Enemy || !Enemy->GetEnemyData()) return EBTNodeResult::Failed;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return EBTNodeResult::Failed;

	ACAPlayerCharacter* Player = Cast<ACAPlayerCharacter>(BB->GetValueAsObject(TEXT("PlayerActor")));
	if (!Player || !Player->IsStaggered()) return EBTNodeResult::Failed;

	if (FVector::Dist2D(Enemy->GetActorLocation(), Player->GetActorLocation()) > MaxRange)
	{
		return EBTNodeResult::Failed;
	}

	UAnimMontage* Montage = Enemy->GetEnemyData()->StaggerPunishMontage;
	
	UE_LOG(LogTemp, Warning, TEXT("StaggerPunish: staggered=%d montage=%s"),
	Player->IsStaggered(), *GetNameSafe(Enemy->GetEnemyData()->StaggerPunishMontage));
	
	if (!Montage) return EBTNodeResult::Failed;

	AIC->StopMovement();
	
	FVector ToPlayer = (Player->GetActorLocation() - Enemy->GetActorLocation()).GetSafeNormal2D();
	Enemy->SetActorRotation(ToPlayer.Rotation());

	if (UAnimInstance* Anim = Enemy->GetMesh()->GetAnimInstance())
	{
		PunishEndTime = Enemy->GetWorld()->GetTimeSeconds() + Anim->Montage_Play(Montage);
	}

	return EBTNodeResult::InProgress;
}

void UCABTTask_StaggerPunish::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (OwnerComp.GetWorld()->GetTimeSeconds() >= PunishEndTime)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
