
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CACharacterMovementComponent.generated.h"

class ACAPlayerCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COMBATARENA_API UCACharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(VisibleAnywhere, Category = "Movement")
	bool bIsSprinting;
	
	UPROPERTY(VisibleAnywhere, Category = "Movement")
	bool bCanDodge = true;
	
	//Time when dodge input was buffered
	float BufferedInputTime = 0.0f;
	
	//Whether a dodge input is waiting to fire
	bool bHasDodgeBuffered = false;
	
	UPROPERTY()
	TObjectPtr<ACAPlayerCharacter> CachedOwner;
	
	virtual void BeginPlay() override;

	
public:
	
	UCACharacterMovementComponent();
	
	void StartSprinting();
	
	void StopSprinting();
	
	void Dodge(); 
	
	// Converts a signed angle (-180 to 180, relative to facing) into a dodge montage section name
	FName GetDodgeSectionForAngle(float AngleDegrees) const;
	
	UPROPERTY(EditDefaultsOnly, Category = "Dodge")
	float SprintDodgeImpulseMultiplier = 2.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Dodge")
	float DodgeMontagePlayRate = 1.3f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Dodge")
	TObjectPtr<UAnimMontage> DodgeMontage;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

private:
	FTimerHandle DodgeTimerHandle;
};
