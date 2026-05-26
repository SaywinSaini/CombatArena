
#include "CAHitstopComponent.h"


UCAHitstopComponent::UCAHitstopComponent()
{
	PrimaryComponentTick.bCanEverTick = false;


}

void UCAHitstopComponent::ApplyHitstop(AActor* HitActor)
{
	if (HitActor == nullptr) return;
	//freeze enemy
	HitActor->CustomTimeDilation = HitstopTimeDilation;
	// freeze attacker/player
	GetOwner()->CustomTimeDilation = HitstopTimeDilation;
	
	TWeakObjectPtr<AActor> WeakHitActor(HitActor);
	
	GetWorld()->GetTimerManager().SetTimer(HitstopTimerHandle,[this,WeakHitActor]()
	{
		if (WeakHitActor.IsValid())
		{
			ResetHitstop(WeakHitActor.Get());
		}
	},HitstopDuration,false);
}

void UCAHitstopComponent::ResetHitstop(AActor* HitActor)
{
	if (HitActor == nullptr) return;
	
	HitActor->CustomTimeDilation = 1.0f;
	GetOwner()->CustomTimeDilation = 1.0f;
}


