// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "CABTTask_MoveToSlot.generated.h"

/**
 * Single owner of melee approach movement. Paths to the enemy's SlotActor via the
 * navigation system (never StopMovement / AddMovementInput), so a moving slot is
 * followed natively. Succeeds once the player is within attack range; the BT's
 * Attack branch takes over from there.
 */
UCLASS()
class COMBATARENA_API UCABTTask_MoveToSlot : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UCABTTask_MoveToSlot();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	// Acceptance radius handed to MoveToActor. Matches the old Chase MoveTo (80) so
	// behaviour doesn't jump. Designer-tunable per BT node instance.
	UPROPERTY(EditAnywhere, Category = "Slot")
	float AcceptanceRadius = 80.f;
};