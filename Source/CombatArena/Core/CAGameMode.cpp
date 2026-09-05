#include "CAGameMode.h"

#include "AI/CAEnemyMovementState.h"
#include "Characters/CAEnemyData.h"
#include "AI/CAEnemyBase.h"
#include "Blueprint/UserWidget.h"
#include "Core/CASpawnPoint.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "UI/CABossHealthBarWidget.h"
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

void ACAGameMode::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACASpawnPoint::StaticClass(), Found);
	SpawnPoints = Found;

	if (SpawnPoints.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("CAGameMode: no spawn points placed in the level"));
		return;
	}

	SpawnWave(0);
}

void ACAGameMode::ShowBossBar(ACAEnemyBase* Boss)
{
	if (!Boss || !BossBarClass || BossBarInstance) return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC) return;

	BossBarInstance = CreateWidget<UCABossHealthBarWidget>(PC, BossBarClass);
	if (!BossBarInstance) return;

	BossBarInstance->AddToViewport();
	BossBarInstance->InitializeForBoss(Boss);
}

void ACAGameMode::HideBossBar()
{
	if (!BossBarInstance) return;

	BossBarInstance->RemoveFromParent();
	BossBarInstance = nullptr;
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

	const UCAEnemyData* Data = Enemy->GetEnemyData();
	if (Data && Data->bIsBoss)
	{
		TWeakObjectPtr<ACAEnemyBase> WeakBoss = Enemy;
		GetWorldTimerManager().SetTimerForNextTick([this, WeakBoss]()
		{
			if (WeakBoss.IsValid())
			{
				ShowBossBar(WeakBoss.Get());
			}
		});
	}
}

void ACAGameMode::UnregisterEnemy(ACAEnemyBase* Enemy)
{
	if (!Enemy) return;
	ActiveEnemies.Remove(Enemy);

	const UCAEnemyData* Data = Enemy->GetEnemyData();
	if (Data && Data->bIsBoss)
	{
		HideBossBar();
	}
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
void ACAGameMode::SpawnWave(int32 WaveIndex)
{
	if (!Waves.IsValidIndex(WaveIndex)) return;

	CurrentWaveIndex = WaveIndex;

	int32 PointIndex = 0;

	for (const FCAWaveEntry& Entry : Waves[WaveIndex].Entries)
	{
		if (!Entry.EnemyClass) continue;

		for (int32 i = 0; i < Entry.Count; ++i)
		{
			// Cycle the points so more enemies than points still spawn.
			AActor* Point = SpawnPoints[PointIndex % SpawnPoints.Num()];
			++PointIndex;

			FActorSpawnParameters Params;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			GetWorld()->SpawnActor<ACAEnemyBase>(Entry.EnemyClass, Point->GetActorTransform(), Params);
		}
	}

	// Enemies register on BeginPlay, so start checking after a short delay.
	GetWorldTimerManager().SetTimer(WaveCheckHandle, this, &ACAGameMode::CheckWaveComplete, 1.0f, true, 2.0f);
}
void ACAGameMode::CheckWaveComplete()
{
	if (GetActiveEnemies().Num() > 0) return;

	GetWorldTimerManager().ClearTimer(WaveCheckHandle);
	OnWaveCleared();
}
void ACAGameMode::OnWaveCleared()
{
	const int32 NextWave = CurrentWaveIndex + 1;

	if (!Waves.IsValidIndex(NextWave))
	{
		UE_LOG(LogTemp, Warning, TEXT("All waves cleared"));
		return;
	}

	GetWorldTimerManager().SetTimer(NextWaveHandle, [this, NextWave]()
	{
		SpawnWave(NextWave);
	}, DelayBetweenWaves, false);
}