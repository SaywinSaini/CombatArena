#include "CAHitDetectionComponent.h"


UCAHitDetectionComponent::UCAHitDetectionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCAHitDetectionComponent::StartTrace()
{
	bIsTracing = true;
	HitActors.Empty();
}

void UCAHitDetectionComponent::StopTrace()
{
	bIsTracing = false;
}

void UCAHitDetectionComponent::PerformTrace()
{
	if (bIsTracing)
	{
		FVector StartPoint = GetOwner()->GetActorLocation();
		
		FVector ForwardPoint = GetOwner()->GetActorForwardVector();
		
		FVector EndPoint = StartPoint + ForwardPoint * TraceRange;
		
		TArray<FHitResult> HitResults;
		
		FCollisionShape Sphere = FCollisionShape::MakeSphere(TraceRadius);
		
		GetWorld()->SweepMultiByChannel(HitResults, StartPoint, EndPoint, FQuat(),ECC_Pawn,Sphere);
		
		for (FHitResult Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();
			if (HitActor && HitActor != GetOwner() && !HitActors.Contains(HitActor))
			{
				HitActors.Add(HitActor);
			}
		}
		
		
	}
}

void UCAHitDetectionComponent::BeginPlay()
{
	Super::BeginPlay();
}

