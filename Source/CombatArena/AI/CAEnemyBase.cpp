
#include "CAEnemyBase.h"
#include "AbilitySystemComponent.h"
#include "CAEnemyAIController.h"
#include "Characters/CAEnemyData.h"
#include "Abilities/CAAttributeSet.h"
#include "GameFramework/CharacterMovementComponent.h"


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
	
	if (EnemyData)
	{
		GetCharacterMovement()->MaxWalkSpeed = EnemyData->MovementSpeed;
		
		//Initialize Health attribute from DataAsset
		AbilitySystemComponent->SetNumericAttributeBase(UCAAttributeSet::GetHealthAttribute(),EnemyData->MaxHealth);
		AbilitySystemComponent->SetNumericAttributeBase(UCAAttributeSet::GetMaxHealthAttribute(),EnemyData->MaxHealth);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("CAEnemyBase: EnemyData is null on %s"), *GetName());
	}
}

void ACAEnemyBase::Die()
{
	UE_LOG(LogTemp,Warning,TEXT("Die : Working"));
}

