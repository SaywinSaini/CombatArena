
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

void UCACharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	
}


void UCACharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                  FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	// Get owning player character to access CharacterData
	ACAPlayerCharacter* Owner =Cast<ACAPlayerCharacter>(GetCharacterOwner());
	if (Owner && Owner->GetCharacterData())
	{
		const float TargetSpeed = bIsSprinting ?
		Owner->GetCharacterData()->SprintSpeed :
		Owner->GetCharacterData()->MovementSpeed;
		
		// Smoothly interpolate MaxWalkSpeed toward target speed each frame
		MaxWalkSpeed = FMath::FInterpTo(MaxWalkSpeed, TargetSpeed,DeltaTime,SpeedInterpSpeed);
	}
	 
}

