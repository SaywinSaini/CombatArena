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
	
	void SetTraceSocket(FName SocketName);

	void ResetTraceSocket();
	
	void SetHitReactSection(FName SectionName);
	
	void SetDeathSection(FName SectionName);
	
	void SetStunAmount(float Amount);
	
	void SetBlockedStunAmount(float Amount);
	
	void SetReactWhileStaggered(bool bAllow);
	
	private:
	
	// Tracks actors already hit this swing to prevent multiple hits per attack
	TSet<TWeakObjectPtr<AActor>> HitActors;
	
	UPROPERTY()
	TObjectPtr<UCACharacterData> CharacterData;
	
	// Trace config, populated in BeginPlay from whichever data asset the owner has
	FName TraceSocketName;
	float TraceRange = 50.f;
	float TraceRadius = 20.f;
	
	//GameplayEffect applied to hit actors , set in editor
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
	bool bIsTracing = false;
	
	bool bIsPlayerFrozen = false;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<UCameraShakeBase> HitCameraShake;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<UCameraShakeBase> BlockCameraShake;
	
	FName DefaultTraceSocketName;
	
	FName PendingHitReactSection;
	
	FName PendingDeathSection;
	
	float PendingStunAmount = 0.f;
	
	float PendingBlockedStunAmount = 0.f;
	
	bool bPendingReactWhileStaggered = false;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TArray<TObjectPtr<USoundBase>> HitSounds;
	
	bool bPlayedHitSoundThisSwing = false;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TObjectPtr<USoundBase> BlockSound;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TArray<TObjectPtr<USoundBase>> SwooshSounds;
	
};
