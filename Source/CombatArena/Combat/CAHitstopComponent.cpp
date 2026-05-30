
#include "CAHitstopComponent.h"


UCAHitstopComponent::UCAHitstopComponent()
{
	PrimaryComponentTick.bCanEverTick = false;


}

void UCAHitstopComponent::ApplyHitstop(AActor* HitActor,bool bFreezePlayer,AActor* PlayerActor)
{
	if (HitActor == nullptr) return;
	//freeze enemy
	HitActor->CustomTimeDilation = HitstopTimeDilation;
	
	//freeze player
	if (bFreezePlayer && PlayerActor)
	{
		PlayerActor->CustomTimeDilation = HitstopTimeDilation;
	}
	
	TWeakObjectPtr<AActor> WeakHitActor(HitActor);
	TWeakObjectPtr<AActor> WeakPlayer(PlayerActor);
	
	GetWorld()->GetTimerManager().SetTimer(HitstopTimerHandle,[this,WeakHitActor,WeakPlayer,bFreezePlayer]()
	{
		if (WeakHitActor.IsValid())
		{
			ResetHitstop(WeakHitActor.Get(),bFreezePlayer,WeakPlayer.Get());
		}
	},HitstopDuration,false);
}

void UCAHitstopComponent::ResetHitstop(AActor* HitActor, bool bFreezePlayer,AActor* PlayerActor)
{
	if (HitActor == nullptr) return;
	
	HitActor->CustomTimeDilation = 1.0f;
	if (bFreezePlayer && PlayerActor)
	{
		PlayerActor->CustomTimeDilation = 1.0f;
	}
}


