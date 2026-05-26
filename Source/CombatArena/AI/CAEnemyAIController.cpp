
#include "CAEnemyAIController.h"

#include "CAEnemyBase.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/CAEnemyData.h"
#include "Characters/CAPlayerCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"


ACAEnemyAIController::ACAEnemyAIController()
{
	UAIPerceptionComponent* AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	SetPerceptionComponent(*AIPerception);
	
	// Create and configure sight sense
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
	
	if (AIPerception)
	{
		AIPerception->ConfigureSense(*SightConfig);
		AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());
		AIPerception->OnTargetPerceptionUpdated.AddDynamic(this,&ACAEnemyAIController::OnTargetPerceptionUpdated);
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
		
		UAIPerceptionComponent* AIPerception = GetPerceptionComponent();
		if (AIPerception)
		{
			AIPerception->ConfigureSense(*SightConfig);
		}
		
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("CAEnemyAIController: EnemyData not assigned on %s"),
		   *InPawn->GetName());
	}
	if (BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("CAEnemyAIController: BehaviorTree not assigned on %s"),
			*GetName());
	}
}

void ACAEnemyAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Actor) return;

	if (!Actor->IsA<ACAPlayerCharacter>()) return;
	
	
	if (Stimulus.WasSuccessfullySensed())
	{
		UE_LOG(LogTemp, Warning, TEXT("CAEnemyAIController: Player spotted — %s"), 
			*Actor->GetName());

		if (GetBlackboardComponent())
		{
			GetBlackboardComponent()->SetValueAsObject(TEXT("PlayerActor"), Actor);
			GetBlackboardComponent()->SetValueAsBool(TEXT("bCanSeePlayer"), true);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CAEnemyAIController: Player lost — %s"), 
			*Actor->GetName());

		if (GetBlackboardComponent())
		{
			// Keep PlayerActor reference for 2 seconds — allows current movement to complete
			GetBlackboardComponent()->SetValueAsBool(TEXT("bCanSeePlayer"), false);
            
			FTimerHandle LostSightTimer;
			GetWorldTimerManager().SetTimer(LostSightTimer, [this]()
			{
				if (GetBlackboardComponent())
				{
					GetBlackboardComponent()->SetValueAsObject(TEXT("PlayerActor"), nullptr);
				}
			}, 2.0f, false);
		}
	}
}

