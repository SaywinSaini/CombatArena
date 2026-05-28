#include "CATargetingComponent.h"

#include "AI/CAEnemyBase.h"
#include "Kismet/GameplayStatics.h"
#include "Splines/SplineMath.h"
#include "GameFramework/PlayerController.h"

UCATargetingComponent::UCATargetingComponent()
{

	PrimaryComponentTick.bCanEverTick = true;

}

void UCATargetingComponent::BeginPlay()
{
	Super::BeginPlay();
	
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
	float BestDot = -1.0f;
	
	FVector OwnerForward = GetOwner()->GetActorForwardVector();
	float ConeThreshold = FMath::Cos(FMath::DegreesToRadians(TargetingConeAngle * 0.5f));
	
	for (AActor* Actor : Enemy)
	{
		if (!Actor) continue;
		
		FVector DirectionToActor = (Actor->GetActorLocation() - GetOwner()->GetActorLocation()).GetSafeNormal();
		float Dot = FVector::DotProduct(OwnerForward, DirectionToActor);
		
		if (Dot < ConeThreshold) continue;
		
		if (Dot > BestDot)
		{
			BestDot = Dot;
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
	
	if (!bIsTargetLocked || !LockedTarget) return;
	
	if (!IsValid(LockedTarget))
	{
		LockedTarget = nullptr;
		bIsTargetLocked=false;
		return;
	}
	RotateTowardsTarget(LockedTarget,DeltaTime);
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

