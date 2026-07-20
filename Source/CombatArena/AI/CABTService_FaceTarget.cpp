#include "CABTService_FaceTarget.h"
#include "AIController.h"
#include "CAEnemyBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/CAPlayerCharacter.h"

UCABTService_FaceTarget::UCABTService_FaceTarget()
{
	NodeName = TEXT("Face Target");
	Interval = 0.1f;          // tick every 0.1s
	RandomDeviation = 0.0f;   // no randomness in the interval
}

void UCABTService_FaceTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	AAIController* AIC = Cast<AAIController>(OwnerComp.GetAIOwner());
	if (!AIC) return;
	
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return;
	
	ACAPlayerCharacter* Player = Cast<ACAPlayerCharacter>(BB->GetValueAsObject("PlayerActor"));
	if (!Player) return;
	
		AIC->SetFocus(Player);
	
}

void UCABTService_FaceTarget::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnCeaseRelevant(OwnerComp, NodeMemory);
	
	
	if (AAIController* AIC = OwnerComp.GetAIOwner())
	{
		AIC->ClearFocus(EAIFocusPriority::Gameplay);
	}
}
