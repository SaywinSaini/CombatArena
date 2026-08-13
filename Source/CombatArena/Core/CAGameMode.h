// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CAGameMode.generated.h"

class ACAEnemyBase;
enum class EEnemyType : uint8;

USTRUCT(BlueprintType)
struct FCAWaveEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Wave")
	TSubclassOf<ACAEnemyBase> EnemyClass;

	UPROPERTY(EditAnywhere, Category = "Wave")
	int32 Count = 1;
};

USTRUCT(BlueprintType)
struct FCAWave
{
	GENERATED_BODY()

	// A wave may mix enemy types, e.g. two Base and one Tank.
	UPROPERTY(EditAnywhere, Category = "Wave")
	TArray<FCAWaveEntry> Entries;
};

UCLASS()
class COMBATARENA_API ACAGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
	public:
	ACAGameMode();
	
	bool TryClaimAttackToken(AActor* Claimant);

	void ReleaseAttackToken(AActor* Claimant);
	
	void RegisterEnemy(ACAEnemyBase* Enemy);
	
	void UnregisterEnemy(ACAEnemyBase* Enemy);
	
	const TArray<TWeakObjectPtr<ACAEnemyBase>>& GetActiveEnemies();
	
	void GetFormationSlot(const ACAEnemyBase* Enemy, int32& OutIndex, int32& OutCount);
	
	void OnWaveCleared();
	
private:
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	int32 MaxConcurrentAttackers = 2;

	UPROPERTY()
	TSet<TWeakObjectPtr<AActor>> AttackTokenHolders;
	
	TArray<TWeakObjectPtr<ACAEnemyBase>> ActiveEnemies;
	
	UPROPERTY(EditDefaultsOnly, Category = "Waves")
	TArray<FCAWave> Waves;

	UPROPERTY(EditDefaultsOnly, Category = "Waves")
	float DelayBetweenWaves = 3.0f;

	void SpawnWave(int32 WaveIndex);

	void CheckWaveComplete();

	UPROPERTY()
	TArray<TObjectPtr<AActor>> SpawnPoints;

	int32 CurrentWaveIndex = -1;

	FTimerHandle WaveCheckHandle;

	FTimerHandle NextWaveHandle;

	virtual void BeginPlay() override;
	
};
