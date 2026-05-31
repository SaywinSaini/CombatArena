
#include "CAEnemyBase.h"
#include "AbilitySystemComponent.h"
#include "BrainComponent.h"
#include "CAEnemyAIController.h"
#include "Characters/CAEnemyData.h"
#include "Abilities/CAAttributeSet.h"
#include "Combat/CAHitstopComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


ACAEnemyBase::ACAEnemyBase()
{
	PrimaryActorTick.bCanEverTick = false;
	
	// Automatically spawned and assigned by the engine when this pawn enters the world
	AIControllerClass = ACAEnemyAIController::StaticClass();
	
   AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AttributeSet = CreateDefaultSubobject<UCAAttributeSet>(TEXT("AttributeSet"));
	
	HitstopComponent = CreateDefaultSubobject<UCAHitstopComponent>(TEXT("HitstopComponent"));
}

void ACAEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	AbilitySystemComponent->InitAbilityActorInfo(this,this);
	
	if (EnemyData)
	{
		GetCharacterMovement()->MaxWalkSpeed = EnemyData->MovementSpeed;
		// Delay one frame — ensures ASC is fully initialized before setting attributes
		GetWorldTimerManager().SetTimerForNextTick([this]()
			{
			if (AbilitySystemComponent && EnemyData)
			{
				//Initialize Health attribute from DataAsset
				AbilitySystemComponent->SetNumericAttributeBase(UCAAttributeSet::GetHealthAttribute(),EnemyData->MaxHealth);
				AbilitySystemComponent->SetNumericAttributeBase(UCAAttributeSet::GetMaxHealthAttribute(),EnemyData->MaxHealth);
			}
	});
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("CAEnemyBase: EnemyData is null on %s"), *GetName());
	}
}

void ACAEnemyBase::Die()
{
	UE_LOG(LogTemp,Warning,TEXT("Die : Working"));
	
	AAIController* AIController = Cast<AAIController>(GetController());
	
	if (AIController)
	{
		AIController->GetBrainComponent()->StopLogic(TEXT("Dead"));
	}
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
	SetLifeSpan(3.0f);
	
}

