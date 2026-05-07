
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CACharacterMovementComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COMBATARENA_API UCACharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(VisibleAnywhere, Category = "Movement")
	bool bIsSprinting;
	
	// Interpolation speed for transitioning between walk and sprint
	UPROPERTY(EditDefaultsOnly,Category= "Movement")
	float SpeedInterpSpeed = 10.0f;
	
	virtual void BeginPlay() override;

	
public:
	
	UCACharacterMovementComponent();
	
	void StartSprinting();
	
	void StopSprinting();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

};
