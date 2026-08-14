// Fill out your copyright notice in the Description page of Project Settings.


#include "CABTTask_Slip.h"

#include "AIController.h"
#include "CAEnemyBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/CAEnemyData.h"

UCABTTask_Slip::UCABTTask_Slip()
{
	NodeName = TEXT("Slip");
	bNotifyTick = false;
}

EBTNodeResult::Type UCABTTask_Slip::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC) return EBTNodeResult::Failed;
	
	ACAEnemyBase* Enemy = Cast<ACAEnemyBase>(AIC->GetPawn());
	if (!Enemy || !Enemy->GetEnemyData()) return EBTNodeResult::Failed;
	
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return EBTNodeResult::Failed;
	
	AActor* Player = Cast<AActor>(BB->GetValueAsObject(TEXT("PlayerActor")));
	if (!Player) return EBTNodeResult::Failed;
	
	const float Distance = FVector::Dist2D(Enemy->GetActorLocation(), Player->GetActorLocation());
	if (Distance > MaxSlipDistance) return EBTNodeResult::Failed;
	
	const UCAEnemyData* Data = Enemy->GetEnemyData();
	if (!Data->SlipMontage) return EBTNodeResult::Failed;
	
	const float Now = Enemy->GetWorld()->GetTimeSeconds();
	if (Now - Enemy->GetLastSlipTime() < Data->SlipCooldown) return EBTNodeResult::Failed;
	
	Enemy->SetLastSlipTime(Now);
	if (FMath::FRand() > SlipChance) return EBTNodeResult::Failed;
	
	const FVector Right = Enemy->GetActorRightVector().GetSafeNormal2D();
	const FVector ToPlayer = (Player->GetActorLocation() - Enemy->GetActorLocation()).GetSafeNormal2D();
	const FName Section = FMath::RandBool() ? FName("Left") : FName("Right");
	
	if (UAnimInstance* Anim = Enemy->GetMesh()->GetAnimInstance())
	{
		const int32 SectionIndex = Data->SlipMontage->GetSectionIndex(Section);
		if (SectionIndex == INDEX_NONE) return EBTNodeResult::Failed;
		
		float Start = 0.f, End = 0.f;
		Data->SlipMontage->GetSectionStartAndEndTime(SectionIndex, Start, End);
		
		Anim->Montage_Play(Data->SlipMontage, SlipPlayRate, EMontagePlayReturnType::MontageLength, Start);
		
		const float Duration = (End - Start) / SlipPlayRate;

		Enemy->SetReacting(Duration);
		Enemy->ApplyDashInvulnerability(Duration);
	}
	
	
	Enemy->ApplyDashInvulnerability(InvulnerabilityDuration);
	
	return EBTNodeResult::Succeeded;
	
}
