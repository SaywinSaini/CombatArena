#include "CASteeringComponent.h"
#include "CAEnemyBase.h"
#include "Core/CAGameMode.h"
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

FVector UCASteeringComponent::GetBestDirection(const AActor* Target, float BandMin, float BandMax)
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
	const bool bDebugDraw = CVarSteeringDebugDrawing.GetValueOnGameThread();

	TArray<FVector> AllyLocations;
	if (ACAGameMode* GameMode = Cast<ACAGameMode>(GetWorld()->GetAuthGameMode()))
	{
		for (const TWeakObjectPtr<ACAEnemyBase>& Ally : GameMode->GetActiveEnemies())
		{
			if (!Ally.IsValid() || Ally.Get() == CachedOwner) continue;

			if (FVector::Dist2D(Ally->GetActorLocation(), OwnerLoc) <= AvoidanceRadius)
			{
				AllyLocations.Add(Ally->GetActorLocation());
			}
		}
	}
	
	int32 SlotIndex = 0;
	int32 SlotCount = 1;
	if (ACAGameMode* GameMode = Cast<ACAGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->GetFormationSlot(CachedOwner, SlotIndex, SlotCount);
	}

	const float SlotAngle = (360.0f / SlotCount) * SlotIndex;
	const FVector TargetForward = Target->GetActorForwardVector().GetSafeNormal2D();
	const FVector DesiredOffset = TargetForward.RotateAngleAxis(SlotAngle, FVector::UpVector);

	auto ScoreDirection = [&](const FVector& Direction) -> float
	{
		const FVector ProbeLoc = OwnerLoc + Direction * ProbeDistance;

		const float ProbeRange = FVector::Dist2D(ProbeLoc, TargetLoc);
		const float RangeError = FMath::Abs(ProbeRange - FMath::Clamp(ProbeRange, BandMin, BandMax));

		float AvoidancePenalty = 0.f;
		for (const FVector& AllyLoc : AllyLocations)
		{
			const FVector ToAlly = (AllyLoc - OwnerLoc).GetSafeNormal2D();
			const float Alignment = FMath::Max(FVector::DotProduct(Direction, ToAlly), 0.f);

			const float Dist = FVector::Dist2D(OwnerLoc, AllyLoc);
			const float Closeness = FMath::Clamp(1.f - (Dist / AvoidanceRadius), 0.f, 1.f);

			AvoidancePenalty += Alignment * Closeness * Closeness * 100.f;
		}
		
		const FVector ToProbe = (ProbeLoc - TargetLoc).GetSafeNormal2D();
		const float FormationError = FMath::RadiansToDegrees(
			FMath::Acos(FMath::Clamp(FVector::DotProduct(DesiredOffset, ToProbe), -1.f, 1.f)));

		return -(RangeError * RangeWeight)
		     - (AvoidancePenalty * AvoidanceWeight)
		     - (FormationError * FormationWeight);
	};

	float BestScore = -FLT_MAX;
	FVector BestDirection = FVector::ZeroVector;

	for (int32 i = 0; i < SampleCount; ++i)
	{
		const float AngleDeg = (360.0f / SampleCount) * i;
		const FVector Direction = FRotator(0.f, AngleDeg, 0.f).Vector();
		const float Score = ScoreDirection(Direction);

		if (Score > BestScore)
		{
			BestScore = Score;
			BestDirection = Direction;
		}

		if (bDebugDraw)
		{
			const float Normalised = FMath::Clamp(1.f - (FMath::Abs(Score) / 200.f), 0.f, 1.f);
			const FColor LineColor = FColor(255 * (1.f - Normalised), 255 * Normalised, 0);
			DrawDebugLine(GetWorld(), OwnerLoc, OwnerLoc + Direction * ProbeDistance,
				LineColor, false, ScoreInterval, 0, 3.f);
		}
	}
	
	if (!LastChosenDirection.IsNearlyZero())
	{
		const float CurrentScore = ScoreDirection(LastChosenDirection);
		if (BestScore < CurrentScore + SwitchMargin)
		{
			BestDirection = LastChosenDirection;
		}
	}

	LastChosenDirection = LastChosenDirection.IsNearlyZero()
		? BestDirection
		: FMath::VInterpNormalRotationTo(LastChosenDirection, BestDirection, ScoreInterval, DirectionInterpSpeed);

	if (bDebugDraw)
	{
		DrawDebugLine(GetWorld(), OwnerLoc, OwnerLoc + LastChosenDirection * ProbeDistance * 2.f,
			FColor::Cyan, false, ScoreInterval, 0, 8.f);
	}

	return LastChosenDirection;
}



