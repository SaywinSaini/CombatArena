
#include "CACharacterMovementComponent.h"

#include "CACharacterData.h"
#include "GameFramework/Character.h"
#include "Characters/CAPlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"


UCACharacterMovementComponent::UCACharacterMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCACharacterMovementComponent::StartSprinting()
{
	bIsSprinting = true;
}

void UCACharacterMovementComponent::StopSprinting()
{
	bIsSprinting = false;
}

void UCACharacterMovementComponent::Dodge()
{
	if (!CachedOwner || !CachedOwner->GetCharacterData()) return;
	
	// Buffer dodge input if dodge is still on cooldown
	if (!bCanDodge)
	{
		bHasDodgeBuffered = true;
		BufferedInputTime = GetWorld()->GetTimeSeconds();
		return;
	}
	// Prevent dodge while airborne
	if (!IsMovingOnGround()) return;
	
	FVector DodgeDirection = GetLastInputVector();
	
	// Backstep when no movement input is provided
	if (DodgeDirection.IsNearlyZero())
	{
		DodgeDirection = -GetCharacterOwner()->GetActorForwardVector();
	}
	
	// Signed angle between facing direction and dodge direction, used to pick montage section
	const FVector Forward = GetCharacterOwner()->GetActorForwardVector();
	
	const FVector ForwardNorm = Forward.GetSafeNormal();
	const FVector DodgeNorm = DodgeDirection.GetSafeNormal();

	const float DotResult = FVector::DotProduct(ForwardNorm, DodgeNorm);
	const float UnsignedAngle = UKismetMathLibrary::DegAcos(DotResult);

	// Cross product Z component tells us left vs right on the horizontal plane
	const float CrossZ = FVector::CrossProduct(ForwardNorm, DodgeNorm).Z;

	const float SignedAngle = (CrossZ >= 0.f) ? UnsignedAngle : -UnsignedAngle;
	
	const FName SectionName = GetDodgeSectionForAngle(SignedAngle);
	
	if (DodgeMontage)
	{
		if (USkeletalMeshComponent* Mesh = GetCharacterOwner()->GetMesh())
		{
			if (UAnimInstance* AnimInstance = Mesh->GetAnimInstance())
			{
				AnimInstance->Montage_Play(DodgeMontage,DodgeMontagePlayRate);
				AnimInstance->Montage_JumpToSection(SectionName,DodgeMontage);
			}
		}
	}
	
	
	// Apply dodge impulse immediately using CharacterMovement
	const float ImpulseMultiplier = bIsSprinting ? SprintDodgeImpulseMultiplier : 1.0f;
	GetCharacterOwner()->LaunchCharacter(DodgeDirection * CachedOwner->GetCharacterData()->DodgeImpulse * ImpulseMultiplier , true,true);
		
	bCanDodge = false;
	
	// Re-enable dodging after cooldown expires
	GetWorld()->GetTimerManager().SetTimer(DodgeTimerHandle,[this]() { bCanDodge = true; },CachedOwner->GetCharacterData()->DodgeCooldown,false);
	
}

FName UCACharacterMovementComponent::GetDodgeSectionForAngle(float AngleDegrees) const
{
	if (AngleDegrees >= -22.5f  && AngleDegrees < 22.5f)   return FName("FWD");
	if (AngleDegrees >=  22.5f  && AngleDegrees < 67.5f)   return FName("RFWD");
	if (AngleDegrees >=  67.5f  && AngleDegrees < 112.5f)  return FName("RD");
	if (AngleDegrees >=  112.5f && AngleDegrees < 157.5f)  return FName("RBWD");
	if (AngleDegrees >= -67.5f  && AngleDegrees < -22.5f)   return FName("LFWD");
	if (AngleDegrees >= -112.5f && AngleDegrees < -67.5f)   return FName("L");
	if (AngleDegrees >= -157.5f && AngleDegrees < -112.5f)  return FName("LBWD");
	
	// remaining range: 157.5 to 180 and -157.5 to -180
	return FName("BWD");
}

void UCACharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// Cache typed owner reference to avoid repeated casts
	CachedOwner = Cast<ACAPlayerCharacter>(GetCharacterOwner());
	
	if (!CachedOwner)
	{
		UE_LOG(LogTemp,Warning,TEXT("ACharacterMovementComponent: Owner is not ACAPlayerCharacter"));
	}
	
}


void UCACharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!CachedOwner || !CachedOwner->GetCharacterData()) return;
	
		const float TargetSpeed = bIsSprinting ?
		CachedOwner->GetCharacterData()->SprintSpeed :
		CachedOwner->GetCharacterData()->MovementSpeed;
		
		// Smoothly interpolate MaxWalkSpeed toward target speed each frame
		MaxWalkSpeed = FMath::FInterpTo(MaxWalkSpeed, TargetSpeed,DeltaTime,CachedOwner->GetCharacterData()->SpeedInterpSpeed);
	
	// Execute buffered dodge if cooldown has finished
	if (bHasDodgeBuffered && bCanDodge)
	{
		const float CurrentTime = GetWorld()->GetTimeSeconds();
		
		// Execute only if the buffered input is still within the buffer window
		if (CurrentTime - BufferedInputTime <= CachedOwner->GetCharacterData()->BufferWindow)
		{
			bHasDodgeBuffered = false;
			Dodge();
		}
		else
		{
			bHasDodgeBuffered = false;
		}
	}
	 
}

