// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CAGameMode.generated.h"


enum class EEnemyType : uint8;
enum class EApproachSlot : uint8;

UCLASS()
class COMBATARENA_API ACAGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
	public:
	ACAGameMode();
	
	EApproachSlot ClaimSlot(EEnemyType EnemyType);
	
	void ReleaseSlot(EApproachSlot Slot);
	
	bool TryClaimAttackToken(AActor* Claimant);

	void ReleaseAttackToken(AActor* Claimant);
	
private:
	
	TSet<EApproachSlot> ClaimedSlots;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	int32 MaxConcurrentAttackers = 2;

	UPROPERTY()
	TSet<TWeakObjectPtr<AActor>> AttackTokenHolders;
	
};
