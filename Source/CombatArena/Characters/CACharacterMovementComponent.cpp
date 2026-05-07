
#include "CACharacterMovementComponent.h"

UCACharacterMovementComponent::UCACharacterMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCACharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	
}


void UCACharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                  FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}

