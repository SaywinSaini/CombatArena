#include "CAAnimNotify_TakedownImpact.h"

#include "Characters/CAPlayerCharacter.h"

void UCAAnimNotify_TakedownImpact::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp || !MeshComp->GetOwner()) return;

	if (ACAPlayerCharacter* Player = Cast<ACAPlayerCharacter>(MeshComp->GetOwner()))
	{
		if (bIsWithdrawal)
		{
			Player->OnTakedownWithdraw();
		}
		else
		{
			Player->OnTakedownImpact();
		}
	}
}