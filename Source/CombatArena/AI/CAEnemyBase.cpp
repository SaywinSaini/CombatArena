#include "CAEnemyBase.h"
#include "AbilitySystemComponent.h"
#include "BrainComponent.h"
#include "CAEnemyAIController.h"
#include "CASteeringComponent.h"
#include "Characters/CAEnemyData.h"
#include "Abilities/CAAttributeSet.h"
#include "Combat/CAHitstopComponent.h"
#include "Components/CapsuleComponent.h"
#include "Combat/CAHitDetectionComponent.h"
#include "Core/CAGameMode.h"
#include "Core/CAGameplayTags.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

ACAEnemyBase::ACAEnemyBase()
{
    PrimaryActorTick.bCanEverTick = true;
    AIControllerClass = ACAEnemyAIController::StaticClass();
    AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    AttributeSet = CreateDefaultSubobject<UCAAttributeSet>(TEXT("AttributeSet"));
    HitstopComponent = CreateDefaultSubobject<UCAHitstopComponent>(TEXT("HitstopComponent"));
    
    HitDetectionComponent = CreateDefaultSubobject<UCAHitDetectionComponent>(TEXT("HitDetectionComponent"));
    
    GetCharacterMovement()->bUseRVOAvoidance = true;
    GetCharacterMovement()->AvoidanceConsiderationRadius = 300.0f;
    GetCharacterMovement()->AvoidanceWeight = 0.5f;
    
    SteeringComponent = CreateDefaultSubobject<UCASteeringComponent>(TEXT("SteeringComponent"));
}

void ACAEnemyBase::ApplyDashInvulnerability(float Duration)
{
    if (!AbilitySystemComponent) return;
    
    AbilitySystemComponent->AddLooseGameplayTag(CATags::State_Invulnerable);
    
    if (APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
    {
        GetCapsuleComponent()->IgnoreActorWhenMoving(Player,true);
    }
    
    GetWorldTimerManager().SetTimer(InvulnTimerHandle, [this]()
    {
        if (AbilitySystemComponent)
        {
            AbilitySystemComponent->RemoveLooseGameplayTag(CATags::State_Invulnerable,100);
        }
        if (APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
        {
            GetCapsuleComponent()->IgnoreActorWhenMoving(Player,false);
        }
    },Duration,false);
}

void ACAEnemyBase::PlayHitReact(UAnimMontage* Montage, float PlayRate)
{
    if (!Montage) return;
    
    if (UAnimInstance* Anim = GetMesh()->GetAnimInstance())
    {
        const float Duration = Anim->Montage_Play(Montage,PlayRate);
        bIsReacting = true;
        GetCharacterMovement()->StopMovementImmediately();
    
        GetWorldTimerManager().SetTimer(HitReactTimerHandle, [this]()
        {
            bIsReacting = false;
        },Duration / PlayRate,false);
    }
}


FGenericTeamId ACAEnemyBase::GetGenericTeamId() const
{
    // Player is team 0; enemies share team 1 so they neither damage nor target each other.
    return FGenericTeamId(1);
}

void ACAEnemyBase::BeginPlay()
{
    Super::BeginPlay();
    AbilitySystemComponent->InitAbilityActorInfo(this, this);

    if (EnemyData)
    {
        CachedPlayer = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
        
        GetWorldTimerManager().SetTimerForNextTick([this]()
        {
            if (AbilitySystemComponent && EnemyData)
            {
                AbilitySystemComponent->SetNumericAttributeBase(UCAAttributeSet::GetHealthAttribute(), EnemyData->MaxHealth);
                AbilitySystemComponent->SetNumericAttributeBase(UCAAttributeSet::GetMaxHealthAttribute(), EnemyData->MaxHealth);
            }

        });
        
        if (ACAGameMode* GameMode = Cast<ACAGameMode>(GetWorld()->GetAuthGameMode()))
        {
         GameMode->RegisterEnemy(this);   
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("CAEnemyBase: EnemyData is null on %s"), *GetName());
    }
}

void ACAEnemyBase::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}

void ACAEnemyBase::Die()
{
    AAIController* AIController = Cast<AAIController>(GetController());
    if (AIController)
    {
        AIController->GetBrainComponent()->StopLogic(TEXT("Dead"));
    }
    
    if (ACAGameMode* GameMode = Cast<ACAGameMode>(GetWorld()->GetAuthGameMode()))
    {
        GameMode->UnregisterEnemy(this);
    }

    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SetLifeSpan(3.0f);
}
