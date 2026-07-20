#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/CapsuleComponent.h"
#include "CASlotActor.generated.h"

UCLASS()
class COMBATARENA_API ACASlotActor : public AActor
{
	GENERATED_BODY()

public:
	ACASlotActor()
	{
		PrimaryActorTick.bCanEverTick = false;
        
		CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
		CapsuleComponent->SetCapsuleSize(34.f, 88.f);
		CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SetRootComponent(CapsuleComponent);
	}

private:
	UPROPERTY()
	TObjectPtr<UCapsuleComponent> CapsuleComponent;
};