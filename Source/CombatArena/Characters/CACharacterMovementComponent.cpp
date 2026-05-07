
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
	
	if (!bCanDodge)
	{
		bHasDodgeBuffered = true;
		BufferedInputTime = GetWorld()->GetTimeSeconds();
		return;
	}
	
	FVector DodgeDirection = GetLastInputVector();
	
	if (DodgeDirection.IsNearlyZero())
	{
		DodgeDirection = -GetCharacterOwner()->GetActorForwardVector();
	}
	GetCharacterOwner()->LaunchCharacter(DodgeDirection * CachedOwner->GetCharacterData()->DodgeImpulse , true,true);
		
	bCanDodge = false;
	
	GetWorld()->GetTimerManager().SetTimer(DodgeTimerHandle,[this]() { bCanDodge = true; },CachedOwner->GetCharacterData()->DodgeCooldown,false);
	
}

void UCACharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	
	CachedOwner = Cast<ACAPlayerCharacter>(GetCharacterOwner());
	if (!CachedOwner)
	{
		UE_LOG(LogTemp,Warning,TEXT("ACharacterMovementComponent: Owner is not ACAPlayerCharacter"));
	}
	
}


void UCACharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                  FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!CachedOwner || !CachedOwner->GetCharacterData()) return;
	
		const float TargetSpeed = bIsSprinting ?
		CachedOwner->GetCharacterData()->SprintSpeed :
		CachedOwner->GetCharacterData()->MovementSpeed;
		
		// Smoothly interpolate MaxWalkSpeed toward target speed each frame
		MaxWalkSpeed = FMath::FInterpTo(MaxWalkSpeed, TargetSpeed,DeltaTime,CachedOwner->GetCharacterData()->SpeedInterpSpeed);
	
	if (bHasDodgeBuffered && bCanDodge)
	{
		const float CurrentTime = GetWorld()->GetTimeSeconds();
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

