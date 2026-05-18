
#include "CAEnemyAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"


ACAEnemyAIController::ACAEnemyAIController()
{
	UAIPerceptionComponent* AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	SetPerceptionComponent(*AIPerception);
	
	// Create and configure sight sense
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 500.0f;
	SightConfig->LoseSightRadius = 600.0f;
	SightConfig->PeripheralVisionAngleDegrees = 60.0f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
	
	UE_LOG(LogTemp, Warning, TEXT("CAEnemyAIController: AIPerception — %s"),
		AIPerception ? TEXT("VALID") : TEXT("NULL"));
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
	
	UE_LOG(LogTemp, Warning, TEXT("CAEnemyAIController: OnPossess called on %s"), *InPawn->GetName());
	
	//Start the behaviour tree - drives all AI decision making
	if (BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);
		UE_LOG(LogTemp, Warning, TEXT("CAEnemyAIController: BehaviorTree started"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CAEnemyAIController: BehaviorTree is NULL"));
	}
}

void ACAEnemyAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Actor) return;

	if (Stimulus.WasSuccessfullySensed())
	{
		UE_LOG(LogTemp, Warning, TEXT("CAEnemyAIController: Player spotted — %s"), *Actor->GetName());

		if (GetBlackboardComponent())
		{
			GetBlackboardComponent()->SetValueAsObject(TEXT("PlayerActor"), Actor);
			GetBlackboardComponent()->SetValueAsBool(TEXT("bCanSeePlayer"), true);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CAEnemyAIController: Player lost — %s"), *Actor->GetName());

		if (GetBlackboardComponent())
		{
			GetBlackboardComponent()->SetValueAsObject(TEXT("PlayerActor"), nullptr);
			GetBlackboardComponent()->SetValueAsBool(TEXT("bCanSeePlayer"), false);
		}
	}
}

