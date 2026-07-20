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
	
private:
	
	TSet<EApproachSlot> ClaimedSlots;
};
