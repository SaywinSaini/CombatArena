#include "CATargetingComponent.h"

#include "AI/CAEnemyBase.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Core/CAGameplayTags.h"
#include "GameFramework/PlayerController.h"

UCATargetingComponent::UCATargetingComponent()
{

	PrimaryComponentTick.bCanEverTick = true;

}

void UCATargetingComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// Cache the owning pawn's player controller for lock-on camera updates.
	APawn* Owner = Cast<APawn>(GetOwner());
	if (Owner)
	{
		PlayerController= Cast<APlayerController>(Owner->GetController());
	}
}

AActor* UCATargetingComponent::FindBestTarget()
{
	TArray<AActor*> Enemy;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),ACAEnemyBase::StaticClass(),Enemy);
	
	AActor* BestTarget = nullptr;
	float BestScore = -FLT_MAX;
	
	FVector OwnerForward = GetOwner()->GetActorForwardVector();
	if (PlayerController)
	{
		OwnerForward = PlayerController->GetControlRotation().Vector().GetSafeNormal2D();
	}
	float ConeThreshold = FMath::Cos(FMath::DegreesToRadians(TargetingConeAngle * 0.5f));
	
	for (AActor* Actor : Enemy)
	{
		if (!Actor) continue;
		
		const float Dist = FVector::Dist(Actor->GetActorLocation(), GetOwner()->GetActorLocation());
		if (Dist > TargetSearchRadius) continue;
		
		const FVector DirectionToActor = (Actor->GetActorLocation() - GetOwner()->GetActorLocation()).GetSafeNormal();
		const float Dot = FVector::DotProduct(OwnerForward, DirectionToActor);
		
		if (Dot < ConeThreshold) continue;
		
		const float Proximity = 1.f - FMath::Clamp(Dist / TargetSearchRadius, 0.f, 1.f);
		const float Score = (Dot * AimWeight) + (Proximity * ProximityWeight);
		
		if (Score > BestScore)
		{
			BestScore = Score;
			BestTarget = Actor;
		}
	}
	return BestTarget;
	
}

void UCATargetingComponent::RotateTowardsTarget(AActor* Target,float DeltaTime)
{
	
	FVector DirectionToTarget = (Target->GetActorLocation() - GetOwner()->GetActorLocation()).GetSafeNormal();

	FRotator TargetRotation = FRotationMatrix::MakeFromX(DirectionToTarget).Rotator();
	TargetRotation.Pitch = 0.0f;
	TargetRotation.Roll = 0.0f;
	
	FRotator CurrentRotation = GetOwner()->GetActorRotation();
	FRotator NewRotation = FMath::RInterpTo(CurrentRotation,TargetRotation,DeltaTime,RotationInterpSpeed);
	
	GetOwner()->SetActorRotation(NewRotation);
}

void UCATargetingComponent::ToggleTargetLock()
{
	if (bIsTargetLocked)
	{
		LockedTarget=nullptr;
		bIsTargetLocked=false;
	}
	else
	{
		AActor* Target = FindBestTarget();
		if (Target)
		{
			LockedTarget = Target;
			bIsTargetLocked=true;
		}
	}
	
	
}

void UCATargetingComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	// Maintain target lock state and facing while a valid target is locked.
	if (!bIsTargetLocked || !LockedTarget) return;
	
	if (!IsValid(LockedTarget))
	{
		LockedTarget = nullptr;
		bIsTargetLocked=false;
		return;
	}
	
	bool bIsAttacking = false;
	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(GetOwner()))
	{
		if (UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
		{
			bIsAttacking = ASC->HasMatchingGameplayTag(CATags::State_Attacking);
		}
	}

	if (!bIsAttacking)
	{
		RotateTowardsTarget(LockedTarget, DeltaTime);
	}
	
	if (PlayerController)
	{
		FVector ToEnemy = (LockedTarget->GetActorLocation() - GetOwner()->GetActorLocation()).GetSafeNormal();
		FRotator TargetRotation = ToEnemy.Rotation();
		TargetRotation.Pitch = CameraVerticalOffset;
		
		FRotator CurrentRotation = PlayerController->GetControlRotation();
		FRotator NewRotation = FMath::RInterpTo(CurrentRotation,TargetRotation,DeltaTime,CameraInterpSpeed);
		
		PlayerController->SetControlRotation(NewRotation);
	}
	
}

