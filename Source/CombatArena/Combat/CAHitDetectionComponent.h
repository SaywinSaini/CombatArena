#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CAHitDetectionComponent.generated.h"

class UCameraShakeBase;
class UGameplayEffect;
class UCACharacterData;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COMBATARENA_API UCAHitDetectionComponent : public UActorComponent
{
	GENERATED_BODY()
	
protected:
	
	virtual void BeginPlay() override;
	
public:
	
	UCAHitDetectionComponent();
	
	void StartTrace();
	
	void StopTrace();
	
	void PerformTrace();
	
	private:
	
	// Tracks actors already hit this swing to prevent multiple hits per attack
	TSet<TWeakObjectPtr<AActor>> HitActors;
	
	UPROPERTY()
	TObjectPtr<UCACharacterData> CharacterData;
	
	//GameplayEffect applied to hit actors , set in editor
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
	bool bIsTracing = false;
	
	bool bIsPlayerFrozen = false;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<UCameraShakeBase> HitCameraShake;
	
	
};
