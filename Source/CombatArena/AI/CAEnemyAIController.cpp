
#include "CAEnemyAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"


ACAEnemyAIController::ACAEnemyAIController()
{
	PrimaryActorTick.bCanEverTick = false;
	
	// Create and configure sight sense
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 1000.0f;
	SightConfig->LoseSightRadius = 1200.0f;
	SightConfig->PeripheralVisionAngleDegrees = 60.0f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
	
	UAIPerceptionComponent* AIPerception = GetPerceptionComponent();
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
	
	//Start the behaviour tree - drives all AI decision making
	if (BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);
	}
}

void ACAEnemyAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Actor) return;
	
	UE_LOG(LogTemp, Warning, TEXT("CAEnemyAIController: Perception updated — Actor: %s"),
		*Actor->GetName());
}

