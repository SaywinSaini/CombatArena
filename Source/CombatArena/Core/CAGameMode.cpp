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
