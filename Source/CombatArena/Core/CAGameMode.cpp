#include "CAGameMode.h"

#include "AI/CAEnemyMovementState.h"
#include "Characters/CAEnemyData.h"
#include "AI/CAEnemyBase.h"
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

void ACAGameMode::RegisterEnemy(ACAEnemyBase* Enemy)
{
	if (!Enemy) return;
	ActiveEnemies.AddUnique(Enemy);
}

void ACAGameMode::UnregisterEnemy(ACAEnemyBase* Enemy)
{
	if (!Enemy) return;
	ActiveEnemies.Remove(Enemy);
}

const TArray<TWeakObjectPtr<ACAEnemyBase>>& ACAGameMode::GetActiveEnemies()
{
	ActiveEnemies.RemoveAll([](const TWeakObjectPtr<ACAEnemyBase>& E)
	{
		return !E.IsValid();
	});
	
	return ActiveEnemies;
}

void ACAGameMode::GetFormationSlot(const ACAEnemyBase* Enemy, int32& OutIndex, int32& OutCount)
{
	OutIndex = 0;
	OutCount = 1;

	if (!Enemy) return;

	// Index derives from position in the registry, so death and spawning
	// redistribute survivors without any explicit reassignment.
	const TArray<TWeakObjectPtr<ACAEnemyBase>>& Enemies = GetActiveEnemies();
	OutCount = FMath::Max(Enemies.Num(), 1);

	for (int32 i = 0; i < Enemies.Num(); ++i)
	{
		if (Enemies[i].Get() == Enemy)
		{
			OutIndex = i;
			return;
		}
	}
}