#include "CAEnemyAIController.h"
#include "CAEnemyBase.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/CAEnemyData.h"
#include "Characters/CAPlayerCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "GameFramework/PlayerController.h"
#include "Perception/AISenseConfig_Sight.h"

ACAEnemyAIController::ACAEnemyAIController()
{
    UAIPerceptionComponent* AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
    SetPerceptionComponent(*AIPerception);

    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = false;

    if (AIPerception)
    {
        AIPerception->ConfigureSense(*SightConfig);
        AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());
        AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &ACAEnemyAIController::OnTargetPerceptionUpdated);
    }
}

void ACAEnemyAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    ACAEnemyBase* Enemy = Cast<ACAEnemyBase>(InPawn);

    if (Enemy && Enemy->GetEnemyData())
    {
        SightConfig->SightRadius = Enemy->GetEnemyData()->SightRadius;
        SightConfig->LoseSightRadius = Enemy->GetEnemyData()->LoseSightRadius;
        SightConfig->PeripheralVisionAngleDegrees = Enemy->GetEnemyData()->PeripheralVisionAngle;

        if (UAIPerceptionComponent* Perception = GetPerceptionComponent())
        {
            Perception->ConfigureSense(*SightConfig);
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("CAEnemyAIController: EnemyData not assigned on %s"), *InPawn->GetName());
    }

    if (BehaviorTree)
    {
        RunBehaviorTree(BehaviorTree);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("CAEnemyAIController: BehaviorTree not assigned on %s"), *GetName());
    }
}

void ACAEnemyAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if (!Actor) return;
    if (!Actor->IsA<ACAPlayerCharacter>()) return;

    if (Stimulus.WasSuccessfullySensed())
    {
        if (GetBlackboardComponent())
        {
            GetWorldTimerManager().ClearTimer(LostSightTimer);
            GetBlackboardComponent()->SetValueAsObject(TEXT("PlayerActor"), Actor);
            GetBlackboardComponent()->SetValueAsBool(TEXT("bCanSeePlayer"), true);
            GetBlackboardComponent()->SetValueAsVector(TEXT("LastKnownLocation"), Actor->GetActorLocation());
        }
    }
    else
    {
        if (UBlackboardComponent* BB = GetBlackboardComponent())
        {
            ACAEnemyBase* Enemy = Cast<ACAEnemyBase>(GetPawn());
            if (!Enemy || !Enemy->GetEnemyData()) return;

            const float Distance = FVector::Dist(GetPawn()->GetActorLocation(), Actor->GetActorLocation());

            // Ignore stimulus expiry while the player is still within lose-sight range.
            if (Distance <= Enemy->GetEnemyData()->LoseSightRadius) return;

            BB->SetValueAsBool(TEXT("bCanSeePlayer"), false);
            BB->SetValueAsVector(TEXT("LastKnownLocation"), Actor->GetActorLocation());

            const float MemoryDuration = Enemy->GetEnemyData()->LostSightMemoryDuration;
            GetWorldTimerManager().SetTimer(LostSightTimer, [this]()
            {
                if (GetBlackboardComponent())
                {
                    GetBlackboardComponent()->SetValueAsObject(TEXT("PlayerActor"), nullptr);
                }
                ClearFocus(EAIFocusPriority::Gameplay);
            }, MemoryDuration, false);
        }
    }
}
