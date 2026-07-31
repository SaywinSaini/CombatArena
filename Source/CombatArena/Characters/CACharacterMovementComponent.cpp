
#include "CACharacterMovementComponent.h"
#include "CACharacterData.h"
#include "GameFramework/Character.h"
#include "Characters/CAPlayerCharacter.h"


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
	
	const bool bNoInput = GetLastInputVector().IsNearlyZero();
	
	UAnimMontage* MontageToPlay = (bNoInput && DodgeBackMontage)
		? DodgeBackMontage
		: DodgeForwardMontage;
	
	if (MontageToPlay)
	{
		if (USkeletalMeshComponent* Mesh = GetCharacterOwner()->GetMesh())
		{
			if (UAnimInstance* AnimInstance = Mesh->GetAnimInstance())
			{
				AnimInstance->Montage_Play(MontageToPlay, DodgeMontagePlayRate);
			}
		}
	}
		
	bCanDodge = false;
	
	// Re-enable dodging after cooldown expires
	GetWorld()->GetTimerManager().SetTimer(DodgeTimerHandle,[this]() { bCanDodge = true; },CachedOwner->GetCharacterData()->DodgeCooldown,false);
	
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

