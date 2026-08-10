
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
	
	bool bIsDodging = false;
	
	//Time when dodge input was buffered
	float BufferedInputTime = 0.0f;
	
	//Whether a dodge input is waiting to fire
	bool bHasDodgeBuffered = false;
	
	UPROPERTY()
	TObjectPtr<ACAPlayerCharacter> CachedOwner;
	
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dodge")
	TObjectPtr<UAnimMontage> DodgeForwardMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dodge")
	TObjectPtr<UAnimMontage> DodgeBackMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dodge")
	TObjectPtr<UAnimMontage> DodgeLeftMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dodge")
	TObjectPtr<UAnimMontage> DodgeRightMontage;

public:
	
	UCACharacterMovementComponent();
	
	void StartSprinting();
	
	void StopSprinting();
	
	void Dodge(); 
	
	UPROPERTY(EditDefaultsOnly, Category = "Dodge")
	float DodgeMontagePlayRate = 1.4f;
	
	bool IsDodging() const { return bIsDodging; }
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

private:
	
	FTimerHandle DodgeTimerHandle;
	
};
