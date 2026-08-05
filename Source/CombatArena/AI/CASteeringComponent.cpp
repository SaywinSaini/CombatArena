#include "CASteeringComponent.h"
#include "CAEnemyBase.h"

#include "DrawDebugHelpers.h"

TAutoConsoleVariable<bool> CVarSteeringDebugDrawing(
	TEXT("game.Steering.DebugDraw"), false,
	TEXT("Enable steering component debug rendering. (0 = off, 1 = enabled)"),
	ECVF_Cheat);

UCASteeringComponent::UCASteeringComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UCASteeringComponent::BeginPlay()
{
	Super::BeginPlay();
	
	CachedOwner = Cast<ACAEnemyBase>(GetOwner());
	
	if (!CachedOwner)
	{
		UE_LOG(LogTemp, Warning, TEXT("CASteeringComponent: Owner is not ACAEnemyBase on %s"), *GetNameSafe(GetOwner()));
	}
	
}


FVector UCASteeringComponent::GetBestDirection(const AActor* Target)
{
	if (!CachedOwner || !Target) return FVector::ZeroVector;

	const float Now = GetWorld()->GetTimeSeconds();

	if (Now - LastScoreTime < ScoreInterval)
	{
		return LastChosenDirection;
	}

	LastScoreTime = Now;

	const FVector OwnerLoc  = CachedOwner->GetActorLocation();
	const FVector TargetLoc = Target->GetActorLocation();
	const FEnemyRoleData& Role = CachedOwner->GetRoleData();
	
	const float DesiredRange = (Role.MinRange + Role.MaxRange) * 0.5f;

	float BestScore = -FLT_MAX;
	FVector BestDirection = FVector::ZeroVector;

	const bool bDebugDraw = CVarSteeringDebugDrawing.GetValueOnGameThread();

	for (int32 i = 0; i < SampleCount; ++i)
	{
		const float AngleDeg = (360.0f / SampleCount) * i;
		const FVector Direction = FRotator(0.f, AngleDeg, 0.f).Vector();

		const FVector ProbeLoc = OwnerLoc + Direction * ProbeDistance;
		const float ProbeRange = FVector::Dist2D(ProbeLoc, TargetLoc);
	
		const FVector TargetForward = Target->GetActorForwardVector().GetSafeNormal2D();
		const FVector ToProbe = (ProbeLoc - TargetLoc).GetSafeNormal2D();
		const float ProbeAngle = FMath::RadiansToDegrees(
			FMath::Acos(FMath::Clamp(FVector::DotProduct(TargetForward, ToProbe), -1.f, 1.f)));
		
		const float DesiredAngle = FMath::Clamp(ProbeAngle, Role.MinAngle, Role.MaxAngle);
		const float AngleError = FMath::Abs(ProbeAngle - DesiredAngle);
		
		const float RangeError = FMath::Abs(ProbeRange - DesiredRange);
		const float Score = -(RangeError * RangeWeight) - (AngleError * AngleWeight);
		
		if (Score > BestScore)
		{
			BestScore = Score;
			BestDirection = Direction;
		}
		
		if (bDebugDraw)
		{
			// Green = close to the desired range, red = far from it.
			const float Normalised = FMath::Clamp(1.f - (FMath::Abs(Score) / DesiredRange), 0.f, 1.f);
			const FColor LineColor = FColor(255 * (1.f - Normalised), 255 * Normalised, 0);

			DrawDebugLine(GetWorld(), OwnerLoc, ProbeLoc, LineColor, false, ScoreInterval, 0, 3.f);
		}
	}
	
	if (bDebugDraw)
	{
		DrawDebugLine(GetWorld(), OwnerLoc, OwnerLoc + BestDirection * ProbeDistance * 1.5f,
			FColor::Cyan, false, ScoreInterval, 0, 8.f);
	}

	LastChosenDirection = BestDirection;
	return LastChosenDirection;
}



