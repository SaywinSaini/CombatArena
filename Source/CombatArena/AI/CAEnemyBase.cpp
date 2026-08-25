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
#include "Combat/CAStunComponent.h"
#include "Combat/CAStunComponent.h"
#include "AIController.h"
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
   
    StunComponent = CreateDefaultSubobject<UCAStunComponent>(TEXT("StunComponent"));
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

void ACAEnemyBase::PlayHitReact(UAnimMontage* Montage, float PlayRate, FName Section)
{
    if (!Montage) return;

    if (UAnimInstance* Anim = GetMesh()->GetAnimInstance())
    {
        float Duration = 0.f;

        if (!Section.IsNone())
        {
            const int32 SectionIndex = Montage->GetSectionIndex(Section);
            if (SectionIndex != INDEX_NONE)
            {
                float Start = 0.f, End = 0.f;
                Montage->GetSectionStartAndEndTime(SectionIndex, Start, End);
                
                Anim->Montage_Play(Montage, PlayRate, EMontagePlayReturnType::MontageLength, Start);
                Duration = (End - Start) / PlayRate;
            }
        }
        if (Duration <= 0.f)
        {
            Duration = Anim->Montage_Play(Montage, PlayRate) / PlayRate;
        }

        bIsReacting = true;
        GetCharacterMovement()->StopMovementImmediately();

        GetWorldTimerManager().SetTimer(HitReactTimerHandle, [this]()
        {
            bIsReacting = false;
        }, Duration, false);
    }
}


FGenericTeamId ACAEnemyBase::GetGenericTeamId() const
{
    // Player is team 0; enemies share team 1 so they neither damage nor target each other.
    return FGenericTeamId(1);
}

void ACAEnemyBase::SetPendingDeath(AActor* Killer, FName SectionOverride)
{
    PendingKiller = Killer;
    PendingDeathSection = SectionOverride;
}

void ACAEnemyBase::SetReacting(float Duration)
{
    bIsReacting = true;
    GetCharacterMovement()->StopMovementImmediately();

    GetWorldTimerManager().SetTimer(HitReactTimerHandle, [this]()
    {
        bIsReacting = false;
    }, Duration, false);
}

void ACAEnemyBase::EnterStagger()
{
    if (bIsDead || bIsStaggered) return;
    bIsStaggered = true;

    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->AddLooseGameplayTag(CATags::State_Staggered);
    }
    
    if (AAIController* AIC = Cast<AAIController>(GetController()))
    {
        AIC->GetBrainComponent()->StopLogic(TEXT("Staggered"));
        AIC->ClearFocus(EAIFocusPriority::Gameplay);
    }
    
    GetCharacterMovement()->StopMovementImmediately();
    
    if (const float Dur = PlayMontage(EnemyData ? EnemyData->StaggerEnterMontage : nullptr))
    {
        UAnimInstance* Anim = GetMesh()->GetAnimInstance();
        GetWorldTimerManager().SetTimer(StaggerPauseHandle, [Anim]() {Anim->Montage_Pause(); }, Dur - 0.05f, false);
    }
    
    GetWorldTimerManager().SetTimer(StaggerTimerHandle, this, &ACAEnemyBase::ExitStagger, StaggerDuration, false);
}


void ACAEnemyBase::ExitStagger()
{
    if (bIsDead) return;

    bIsStaggered = false;

    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->RemoveLooseGameplayTag(CATags::State_Staggered, 100);
    }

    if (StunComponent)
    {
        StunComponent->ResetStun();
    }

    const float ExitDuration = PlayMontage(EnemyData ? EnemyData->StaggerExitMontage : nullptr);

    if (ExitDuration > 0.f)
    {
        GetWorldTimerManager().SetTimer(StaggerTimerHandle, this, &ACAEnemyBase::RestartBrain, ExitDuration, false);
    }
    else
    {
        RestartBrain();
    }
}

float ACAEnemyBase::PlayMontage(UAnimMontage* Montage, float PlayRate)
{
    if (!Montage) return 0.f;

    UAnimInstance* Anim = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
    return Anim ? Anim->Montage_Play(Montage, PlayRate) : 0.f;
}

void ACAEnemyBase::RestartBrain()
{
    if (AAIController* AIC = Cast<AAIController>(GetController()))
    {
        AIC->GetBrainComponent()->RestartLogic();
    }
}


void ACAEnemyBase::PlayTakedownVictim()
{
    if (bIsDead) return;
    
    GetWorldTimerManager().ClearTimer(StaggerTimerHandle);
    GetWorldTimerManager().ClearTimer(StaggerPauseHandle);

    if (AAIController* AIC = Cast<AAIController>(GetController()))
    {
        AIC->GetBrainComponent()->StopLogic(TEXT("Takedown"));
    }

    GetCharacterMovement()->StopMovementImmediately();
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
                AbilitySystemComponent->SetNumericAttributeBase(UCAAttributeSet::GetMaxStunAttribute(), 100.f);
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

FName ACAEnemyBase::ResolveDeathSection() const
{
    if (!PendingDeathSection.IsNone()) return PendingDeathSection;

    if (!PendingKiller) return FName("Side");

    const FVector Forward = GetActorForwardVector().GetSafeNormal2D();
    const FVector ToKiller = (PendingKiller->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();

    const float Dot = FMath::Clamp(FVector::DotProduct(Forward, ToKiller), -1.f, 1.f);
    const float Angle = FMath::RadiansToDegrees(FMath::Acos(Dot));
    
    return (Angle > 90.f) ? FName("Front") : FName("Side");
}

void ACAEnemyBase::Die()
{
    if (bIsDead) return;
    bIsDead = true;

    if (ACAGameMode* GameMode = Cast<ACAGameMode>(GetWorld()->GetAuthGameMode()))
    {
        GameMode->UnregisterEnemy(this);
    }

    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    if (EnemyData && EnemyData->DeathMontage)
    {
        if (UAnimInstance* Anim = GetMesh()->GetAnimInstance())
        {
            const FName Section = ResolveDeathSection();

            float Duration = 0.f;
            const int32 SectionIndex = EnemyData->DeathMontage->GetSectionIndex(Section);
            if (SectionIndex != INDEX_NONE)
            {
                float Start = 0.f, End = 0.f;
                EnemyData->DeathMontage->GetSectionStartAndEndTime(SectionIndex, Start, End);

                Anim->Montage_Play(EnemyData->DeathMontage, 1.f, EMontagePlayReturnType::MontageLength, Start);
                Duration = End - Start;
            }

            if (Duration > 0.f)
            {
                FTimerHandle FreezeHandle;
                GetWorldTimerManager().SetTimer(FreezeHandle, [Anim]()
                {
                    Anim->Montage_Pause();
                }, Duration - 0.05f, false);
            }
        }
    }

    if (AAIController* AIController = Cast<AAIController>(GetController()))
    {
        AIController->GetBrainComponent()->StopLogic(TEXT("Dead"));
        AIController->ClearFocus(EAIFocusPriority::Gameplay);
    }

    GetCharacterMovement()->StopMovementImmediately();
    GetCharacterMovement()->DisableMovement();
    GetCharacterMovement()->bOrientRotationToMovement = false;
    GetCharacterMovement()->bUseControllerDesiredRotation = false;

    SetLifeSpan(5.0f);
}
