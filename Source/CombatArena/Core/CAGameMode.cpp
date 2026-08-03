#include "CAGameMode.h"

#include "AI/CAISlot.h"
#include "Characters/CAEnemyData.h"
#include "GameFramework/Actor.h"
#include "UObject/ConstructorHelpers.h"

ACAGameMode::ACAGameMode()
{
    static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClass(
           TEXT("/Game/CombatArena/Characters/Player/BP_PlayerCharacter"));
       
       if (PlayerPawnClass.Class)
       {
           DefaultPawnClass = PlayerPawnClass.Class;
       }
}

EApproachSlot ACAGameMode::ClaimSlot(EEnemyType EnemyType)
{
	TArray<EApproachSlot> EligibleSlots;

	switch (EnemyType)
	{
	case EEnemyType::Base:
		EligibleSlots = { EApproachSlot::Front,EApproachSlot::FrontLeft,EApproachSlot::FrontRight,EApproachSlot::Left,EApproachSlot::Right};
		break;
	case EEnemyType::Aggressive:
		EligibleSlots = {EApproachSlot::Back};
		break;
		
	case EEnemyType::Tank:
		EligibleSlots = {EApproachSlot::Front};
		break;
	}
	
	TArray<EApproachSlot> AvailableSlots;
	for (EApproachSlot Slot : EligibleSlots)
	{
		if (!ClaimedSlots.Contains(Slot))
		{
			AvailableSlots.Add(Slot);
		}
	}
	if (AvailableSlots.IsEmpty()) return EApproachSlot::None;
	
	EApproachSlot Chosen =AvailableSlots[FMath::RandRange(0, AvailableSlots.Num() - 1)];
	ClaimedSlots.Add(Chosen);
	return Chosen;
}

void ACAGameMode::ReleaseSlot(EApproachSlot Slot)
{
	ClaimedSlots.Remove(Slot);
}

bool ACAGameMode::TryClaimAttackToken(AActor* Claimant)
{
	if (!Claimant) return false;

	// Already holding one — re-entrant calls shouldn't consume a second.
	if (AttackTokenHolders.Contains(Claimant)) return true;

	// Drop holders destroyed mid-attack so their tokens aren't leaked.
	for (auto It = AttackTokenHolders.CreateIterator(); It; ++It)
	{
		if (!It->IsValid())
		{
			It.RemoveCurrent();
		}
	}

	if (AttackTokenHolders.Num() >= MaxConcurrentAttackers) return false;

	AttackTokenHolders.Add(Claimant);
	return true;
}

void ACAGameMode::ReleaseAttackToken(AActor* Claimant)
{
	if (!Claimant) return;
	AttackTokenHolders.Remove(Claimant);
}
