
#include "CAEnemyBase.h"
#include "AbilitySystemComponent.h"
#include "CAEnemyAIController.h"
#include "Abilities/CAAttributeSet.h"


ACAEnemyBase::ACAEnemyBase()
{
	PrimaryActorTick.bCanEverTick = false;
	
	// Automatically spawned and assigned by the engine when this pawn enters the world
	AIControllerClass = ACAEnemyAIController::StaticClass();
	
   AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AttributeSet = CreateDefaultSubobject<UCAAttributeSet>(TEXT("AttributeSet"));
}

void ACAEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	AbilitySystemComponent->InitAbilityActorInfo(this,this);
}

void ACAEnemyBase::Die()
{
	UE_LOG(LogTemp,Warning,TEXT("Die : Working"));
}

