
#include "CAHitstopComponent.h"


UCAHitstopComponent::UCAHitstopComponent()
{
	PrimaryComponentTick.bCanEverTick = false;


}

void UCAHitstopComponent::ApplyHitstop(AActor* HitActor,bool bFreezePlayer,AActor* PlayerActor)
{
	if (HitActor == nullptr) return;
	
	if (GetWorld()->GetTimerManager().IsTimerActive(HitstopTimerHandle)) return;
	
	HitActor->CustomTimeDilation = HitstopTimeDilation;
	
	if (bFreezePlayer && PlayerActor)
	{
		PlayerActor->CustomTimeDilation = HitstopTimeDilation;
	}
	
	TWeakObjectPtr<AActor> WeakHitActor(HitActor);
	TWeakObjectPtr<AActor> WeakPlayer(PlayerActor);
	
	// Store weak references because the timer may execute after either actor has been destroyed.
	GetWorld()->GetTimerManager().SetTimer(HitstopTimerHandle,[WeakHitActor,WeakPlayer,bFreezePlayer]()
	{
		if (WeakHitActor.IsValid())
		{
			WeakHitActor->CustomTimeDilation = 1.0f;
		}
		
		if (bFreezePlayer && WeakPlayer.IsValid())
		{
			WeakPlayer->CustomTimeDilation = 1.0f;
		}
	},HitstopDuration,false);
}



