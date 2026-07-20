#include "CAEnemyBase.h"
#include "AbilitySystemComponent.h"
#include "BrainComponent.h"
#include "CAEnemyAIController.h"
#include "Characters/CAEnemyData.h"
#include "Abilities/CAAttributeSet.h"
#include "Combat/CAHitstopComponent.h"
#include "Components/CapsuleComponent.h"
#include "AI/CASlotActor.h"
#include "Core/CAGameMode.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

ACAEnemyBase::ACAEnemyBase()
{
    PrimaryActorTick.bCanEverTick = true;
    AIControllerClass = ACAEnemyAIController::StaticClass();
    AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    AttributeSet = CreateDefaultSubobject<UCAAttributeSet>(TEXT("AttributeSet"));
    HitstopComponent = CreateDefaultSubobject<UCAHitstopComponent>(TEXT("HitstopComponent"));
}

void ACAEnemyBase::BeginPlay()
{
    Super::BeginPlay();
    AbilitySystemComponent->InitAbilityActorInfo(this, this);

    if (EnemyData)
    {
        if (ACAGameMode* GameMode = Cast<ACAGameMode>(GetWorld()->GetAuthGameMode()))
        {
            ClaimedSlot = GameMode->ClaimSlot(EnemyData->EnemyType);
        }

        CachedPlayer = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

        if (ClaimedSlot != EApproachSlot::None)
        {
            FActorSpawnParameters Params;
            Params.Owner = this;
            SlotActor = GetWorld()->SpawnActor<ACASlotActor>(
                ACASlotActor::StaticClass(),
                GetActorLocation(),
                FRotator::ZeroRotator,
                Params);
        }

        GetWorldTimerManager().SetTimerForNextTick([this]()
        {
            if (AbilitySystemComponent && EnemyData)
            {
                AbilitySystemComponent->SetNumericAttributeBase(UCAAttributeSet::GetHealthAttribute(), EnemyData->MaxHealth);
                AbilitySystemComponent->SetNumericAttributeBase(UCAAttributeSet::GetMaxHealthAttribute(), EnemyData->MaxHealth);
            }

            if (SlotActor)
            {
                if (ACAEnemyAIController* AIC = Cast<ACAEnemyAIController>(GetController()))
                {
                    if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
                    {
                        BB->SetValueAsObject(TEXT("SlotActor"), SlotActor);
                    }
                }
            }
        });
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("CAEnemyBase: EnemyData is null on %s"), *GetName());
    }
}

void ACAEnemyBase::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!CachedPlayer || !EnemyData) return;

    const FVector PlayerLocation = CachedPlayer->GetActorLocation();

    // --- Slot positioning (legacy — only runs if a slot is claimed) ---
    if (SlotActor && ClaimedSlot != EApproachSlot::None)
    {
        const FVector PlayerForward = CachedPlayer->GetActorForwardVector();
        const FVector PlayerRight   = CachedPlayer->GetActorRightVector();
        const float   ApproachDistance = EnemyData->SlotApproachDistance;

        FVector SlotOffset = FVector::ZeroVector;
        switch (ClaimedSlot)
        {
        case EApproachSlot::Front:      SlotOffset =  PlayerForward * ApproachDistance; break;
        case EApproachSlot::FrontRight: SlotOffset = (PlayerForward + PlayerRight).GetSafeNormal() * ApproachDistance; break;
        case EApproachSlot::Right:      SlotOffset =  PlayerRight * ApproachDistance; break;
        case EApproachSlot::BackRight:  SlotOffset = (-PlayerForward + PlayerRight).GetSafeNormal() * ApproachDistance; break;
        case EApproachSlot::Back:       SlotOffset = -PlayerForward * ApproachDistance; break;
        case EApproachSlot::BackLeft:   SlotOffset = (-PlayerForward - PlayerRight).GetSafeNormal() * ApproachDistance; break;
        case EApproachSlot::Left:       SlotOffset = -PlayerRight * ApproachDistance; break;
        case EApproachSlot::FrontLeft:  SlotOffset = (PlayerForward - PlayerRight).GetSafeNormal() * ApproachDistance; break;
        default:                        SlotOffset = FVector::ZeroVector; break;
        }
        SlotActor->SetActorLocation(PlayerLocation + SlotOffset);
    }
}

void ACAEnemyBase::Die()
{
    if (ACAGameMode* GameMode = Cast<ACAGameMode>(GetWorld()->GetAuthGameMode()))
    {
        if (SlotActor)
        {
            SlotActor->Destroy();
            SlotActor = nullptr;
        }
        GameMode->ReleaseSlot(ClaimedSlot);
        ClaimedSlot = EApproachSlot::None;
    }

    UE_LOG(LogTemp, Warning, TEXT("Die : Working"));

    AAIController* AIController = Cast<AAIController>(GetController());
    if (AIController)
    {
        AIController->GetBrainComponent()->StopLogic(TEXT("Dead"));
    }

    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SetLifeSpan(3.0f);
}